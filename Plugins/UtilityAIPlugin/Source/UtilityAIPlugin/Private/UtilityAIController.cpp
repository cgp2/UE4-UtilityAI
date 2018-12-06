// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilityAIController.h"
#include "UtilityAIPlugin/PointsOfInterest/PointOfInterestComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/Engine/Selection.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Runtime/NavigationSystem/Public/NavigationPath.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "UtilityAIPlugin.h"
#include <DrawDebugHelpers.h>

class AActor;
class FUtilityAIPluginModule;
class UNavigationSystemV1;

AUtilityAIController::AUtilityAIController()
{
#if WITH_EDITORONLY_DATA
	USelection::SelectObjectEvent.AddUObject(this, &AUtilityAIController::OnObjectSelected);
#endif

	BehaviorGroupManager = CreateDefaultSubobject<UBehaviourGroupManagerComponent>(TEXT("BehaviorGroupManager"));
	IntentionManager = CreateDefaultSubobject<UIntentionManagerComponent>(TEXT("IntentionManager"));
	ActionManager = CreateDefaultSubobject<UActionManagerComponent>(TEXT("ActionManager"));

	RoleName = FName("Base");
	IsStoppedOnOverlap = true;
	IsPathfindingEnabled = true;
	IsProjectedDestinationToNavigation = false;
	CanStrafe = true;
	IsPartialPathAllowed = true;
	IsPaused = false;
	CurrentStage = EAIStage::AS_Waiting;
}

void AUtilityAIController::Initialize_Implementation(bool& isSuccess)
{
	ID = FGuid().NewGuid();
	BehaviorGroupManager->Initialize(isSuccess);
	if (isSuccess)
	{
		ActionManager->Initialize(isSuccess);
		if (isSuccess)
		{
			IntentionManager->Initialize(isSuccess);
			BehaviorGroupManager->PostInitialize(isSuccess);
			IntentionManager->PostInitialize(isSuccess);
		}
	}
	IsInitialized = isSuccess;
}

void AUtilityAIController::StartMovementToTarget(AActor* target)
{
	if (!target->IsValidLowLevel())
		return;

	ChangeAIStage(EAIStage::AS_Moving);
	if (!IsPaused)
	{
		IsTargetReached = false;
		CurrentMovementTarget = target;
		AAreaOfInterest* area = Cast<AAreaOfInterest>(target);

		if (area)
		{
			MovementType = EMovementType::MT_ToArea;
			MoveToArea(area);
		}
		else
		{
			MovementType = EMovementType::MT_ToPoI;
			UPointOfInterestComponent* poi = target->FindComponentByClass<class UPointOfInterestComponent>();
			if (poi)
				MoveToPoint(poi);
			else
				MoveToActor(target);
		}
	}
}

void AUtilityAIController::StartMovementToLocation(FVector location)
{
	CurrentMovementTarget = nullptr;
	CurrentMovementTargetLocation = location;
	IsTargetReached = false;
	MovementType = EMovementType::MT_ToLocation;

	MoveToLocation(location, -1.0f, IsStoppedOnOverlap, true, IsProjectedDestinationToNavigation, CanStrafe, DefaultNavigationFilterClass, IsPartialPathAllowed);
}

void AUtilityAIController::MoveToArea_Implementation(AAreaOfInterest* area)
{
	if (area)
	{
		CurrentMovementTarget = area;

		FVector origin, extents;
		area->GetActorBounds(true, origin, extents);
		FVector randPoint = UKismetMathLibrary::RandomPointInBoundingBox(origin, extents* 0.5);
		//		randPoint = rot.RotateVector(randPoint);
		CurrentMovementTargetLocation = randPoint;
		/*if (DefaultNavigationFilterClass != NULL)
			MoveToLocation(area->GetActorLocation(), -1, true, true, false, true, DefaultNavigationFilterClass, true);
		else
			MoveToLocation(area->GetActorLocation(), -1);*/

		MoveToLocation(randPoint, -1, IsStoppedOnOverlap, IsPathfindingEnabled, IsProjectedDestinationToNavigation, CanStrafe, DefaultNavigationFilterClass, IsPartialPathAllowed);
	}
}

void AUtilityAIController::MoveToPoint_Implementation(UPointOfInterestComponent* poi)
{
	if (poi)
	{
		CurrentMovementTarget = poi->GetOwner();
		CurrentMovementTargetLocation = poi->GetComponentLocation();
		MoveToLocation(poi->GetComponentLocation(), poi->AcceptanceRadius, IsStoppedOnOverlap, IsPathfindingEnabled, IsProjectedDestinationToNavigation, CanStrafe, DefaultNavigationFilterClass, IsPartialPathAllowed);
	}
}

void AUtilityAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
		EndMovement();
}

void AUtilityAIController::EndMovement()
{
	if (!IsPaused)
	{
		if (CurrentMovementTarget->IsValidLowLevel())
		{
			switch (MovementType)
			{
			case EMovementType::MT_ToArea:
				OnMovementFinished.Broadcast(MovementType);
				break;
			case EMovementType::MT_ToPoI:
			{
				UPointOfInterestComponent* poi = CurrentMovementTarget->FindComponentByClass<class UPointOfInterestComponent>();
				if (poi)
				{
					FVector poiLocation = poi->GetComponentLocation();
					FVector actorLocation = GetPawn()->GetActorLocation();
					poiLocation.Z = 0.0f;
					actorLocation.Z = 0.0f;
					float distance = (poiLocation - actorLocation).Size();
					float acceptanceDistance = poi->AcceptanceRadius - distance;
					if (acceptanceDistance <= poi->AcceptanceRadius * MovementPrecision)
					{
						MovementPrecision = 0.2f;
						MovementAttempts = 0;
						IsTargetReached = true;
						poi->ProceedInteraction(GetPawn());
						OnMovementFinished.Broadcast(MovementType);
					}
					else
					{
						MovementAttempts++;
						MovementPrecision += 0.5f;
						if (MovementAttempts > 20)
						{
							MovementFailed(CurrentMovementTarget);
						}
						else
						{
							if (acceptanceDistance > 10)
								StartMovementToTarget(CurrentMovementTarget);
							else
							{
								MovementPrecision = 0.2f;
								MovementAttempts = 0;
								IsTargetReached = true;
								poi->ProceedInteraction(GetPawn());
								OnMovementFinished.Broadcast(MovementType);
							}
						}
					}
				}
				break;
			}
			case EMovementType::MT_ToLocation:
				OnMovementFinished.Broadcast(MovementType);
				break;
			default:
				break;
			}
		}
		else if (MovementType == EMovementType::MT_ToLocation)
			OnMovementFinished.Broadcast(MovementType);
	}
}

bool AUtilityAIController::GetRandomPointInRadius(const FVector& Origin, float Radius, TSubclassOf<UNavigationQueryFilter> NavFilter, FVector& OutResult)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetPawn()->GetWorld());
	if (!NavSys)
	{
		return false;
	}

	FNavLocation Result;
	FVector res = NavSys->GetRandomReachablePointInRadius(GetPawn()->GetWorld(), Origin, Radius, (ANavigationData*)0, NavFilter);

	OutResult = res;
	return true;
}

bool AUtilityAIController::CheckPointReachable(const FVector Point, const FVector Extent)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetPawn()->GetWorld());
	if (!NavSys)
		return false;
	FNavLocation location;
	FSharedConstNavQueryFilter Filter = UNavigationQueryFilter::GetQueryFilter(*NavSys->MainNavData, DefaultNavigationFilterClass);

	//return NavSys->ProjectPointToNavigation(Point, location, Extent, (ANavigationData*)0, DefaultNavigationFilterClass);
	return NavSys->ProjectPointToNavigation(Point, location, Extent, (ANavigationData*)0, Filter);

}

void AUtilityAIController::GetAllAreasOfInterest(TArray<AAreaOfInterest*>& AreasOfInterest)
{
	AreasOfInterest = FUtilityAIPluginModule::Get().AIManager->GetAreasOfInterest();
}

void AUtilityAIController::FindAreaOfInterest(FName AreaName, AAreaOfInterest*& AreaOfInterest)
{
	TArray<AAreaOfInterest*> AreasOfInterest = FUtilityAIPluginModule::Get().AIManager->GetAreasOfInterest();
	for (int32 i = 0; i < AreasOfInterest.Num(); i++)
	{
		if (AreasOfInterest[i]->Name == AreaName)
		{
			AreaOfInterest = AreasOfInterest[i];
			return;
		}
	}
	return;
}

void AUtilityAIController::GetAllPointsOfInterest(TArray<UPointOfInterestComponent*>& PointsOfInterest)
{
	PointsOfInterest = FUtilityAIPluginModule::Get().AIManager->GetPointsOfInterest();
}

void AUtilityAIController::FindPointOfInterest(FName PoIName, UPointOfInterestComponent*& PointOfInterest)
{
	TArray<UPointOfInterestComponent*> PointsOfInterest = FUtilityAIPluginModule::Get().AIManager->GetPointsOfInterest();
	for (int32 i = 0; i < PointsOfInterest.Num(); i++)
	{
		if (PointsOfInterest[i]->Name == PoIName)
		{
			PointOfInterest = PointsOfInterest[i];
			return;
		}
	}
	return;
}

void AUtilityAIController::GetGameState(AGameStateBase*& GameState)
{
	GameState = GetWorld()->GetGameState();
}

void AUtilityAIController::Reappoint_Implementation()
{
	ActionManager->ChooseActions(IntentionManager->GetCurrentIntention());
}

void AUtilityAIController::MovementFailed(AActor* target)
{
	OnMovementFailed.Broadcast(MovementType);
}

void AUtilityAIController::AttachActorToParent(USkeletalMeshComponent* targetMesh, FName socketName, FTransform transform, UStaticMesh* mesh, AStaticMeshActor*& SpawnedActor)
{
	if (targetMesh->IsValidLowLevel() && mesh->IsValidLowLevel())
	{
		AStaticMeshActor* smActor = GetWorld()->SpawnActor<AStaticMeshActor>();
		smActor->SetMobility(EComponentMobility::Movable);
		smActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
		smActor->SetActorTransform(transform);
		smActor->AttachToComponent(targetMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, socketName);
		SpawnedActor = smActor;
	}

}

void AUtilityAIController::PauseAI(float duration /*= -1.0f*/)
{
	IsPaused = true;
	IntentionManager->PauseUpdate();
	ActionManager->PauseActions();
	pausedStage = CurrentStage;
	StopMovement();
	if (duration != -1.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(UPauseTimerHandler, this, &AUtilityAIController::UnpauseAI, duration, false);
	}
}

void AUtilityAIController::UnpauseAI()
{
	IsPaused = false;
	IntentionManager->UnpauseUpdate();
	ActionManager->UnpauseActions();
	UIntentionAbstract* currentIntention = IntentionManager->GetCurrentIntention();

	switch (pausedStage)
	{
	case EAIStage::AS_IntentionChoosing:
		IntentionManager->MakeDecision();
		break;
	case EAIStage::AS_ActionChoosing:
	{
		ActionManager->OnIntentionChanged(currentIntention);
		break;
	}
	case EAIStage::AS_Interaction:
	{
		if (CurrentMovementTarget->IsValidLowLevel())
		{
			UPointOfInterestComponent* poi = CurrentMovementTarget->FindComponentByClass<UPointOfInterestComponent>();
			if (poi->IsValidLowLevel())
			{
				poi->ProceedInteraction(GetPawn());
				break;
			}
		}
		ActionManager->OnIntentionChanged(currentIntention);
		break;
	}
	case EAIStage::AS_Moving:
	{
		if (CurrentMovementTarget->IsValidLowLevel())
		{
			StartMovementToTarget(CurrentMovementTarget);
		}
		else
		{
			ActionManager->OnIntentionChanged(currentIntention);
		}
		break;
	}
	case EAIStage::AS_PoIChoosing:
	{
		float maxUtility;
		FGuid maxID;
		ActionManager->CurrentActions.Last(0)->FindTarget(maxUtility, currentIntention, maxID);
	}
	default:
		break;
	}

	IntentionManager->MakeDecision();
}

void AUtilityAIController::ChangeAIStage(EAIStage NewStage)
{
	if (NewStage != CurrentStage)
		CurrentStage = NewStage;
}

#if WITH_EDITORONLY_DATA
void AUtilityAIController::OnObjectSelected(UObject* Object)
{
	if (Object == GetPawn())
	{
		if (FUtilityAIPluginModule::Get().IsDebug)
		{
			StartDebugMode();
		}
	}
	else if (IsDebugMode)
	{
		EndDebugMode();
	}
}

void AUtilityAIController::StartDebugMode()
{
	GetWorld()->GetTimerManager().SetTimer(DebugTickTimerHandler, this, &AUtilityAIController::DebugModeTick, 0.2f, true);

	IsDebugMode = true;
}

void AUtilityAIController::EndDebugMode()
{
	GetWorld()->GetTimerManager().ClearTimer(DebugTickTimerHandler);
	IsDebugMode = false;
}

void AUtilityAIController::DebugModeTick()
{
	UNavigationPath* navPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(),
		CurrentMovementTargetLocation, this, DefaultNavigationFilterClass);

	for (int32 i = 1; i < navPath->PathPoints.Num(); i++)
	{
		DrawDebugLine(GetWorld(), navPath->PathPoints[i - 1] + FVector(0.0f, 0.0f, 10.0f), navPath->PathPoints[i] + FVector(0.0f, 0.0f, 10.0f), FColor::Blue, true, 0.2f, 0, 5.0f);
	}

	if (CurrentMovementTarget->IsValidLowLevel())
	{
		FVector origin;
		FVector targetBoxExtent;

		CurrentMovementTarget->GetActorBounds(false, origin, targetBoxExtent);
		//DrawDebugSolidBox(GetWorld(), origin, targetBoxExtent, FColor::Blue, true, 0.2f, 0);
		DrawDebugBox(GetWorld(), origin, targetBoxExtent, FColor::Blue, true, 0.2f, 0, 10.0f);
	}
	else
	{
		FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
		traceParams.bTraceComplex = true;
		traceParams.bTraceAsyncScene = true;
		traceParams.bReturnPhysicalMaterial = false;

		FHitResult hitRes(ForceInit);

		GetWorld()->LineTraceSingleByChannel(hitRes,
			CurrentMovementTargetLocation,
			CurrentMovementTargetLocation - FVector(0, 0, 3000),
			ECollisionChannel::ECC_Visibility,
			traceParams);

		DrawDebugCylinder(GetWorld(),
			CurrentMovementTargetLocation,
			hitRes.ImpactPoint,
			35.0f,
			12,
			FColor::Blue, true,
			0.2f,
			0,
			5.0f);
	}
}

#endif

void AUtilityAIController::BeginPlay()
{
	Super::BeginPlay();
	//GetPawn()->GetName();
	if (GetPawn()->IsValidLowLevel())
	{
		Initialize(IsInitialized);
	}
}

//void AUtilityAIController::OnPossess(APawn* PossessedPawn)
//{
//	if (!IsInitialized)
//	{
//		Initialize(IsInitialized);
//	}
//}

bool AUtilityAIController::FloatEqualWithTollerance(float value1, float value2, float tollerance)
{
	if (value1 >= value2 - tollerance && value2 <= value2 + tollerance)
		return true;
	return false;
}

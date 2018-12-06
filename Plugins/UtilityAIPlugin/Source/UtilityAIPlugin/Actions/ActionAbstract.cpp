// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionAbstract.h"
#include "UtilityAIPlugin.h"
#include "UtilityAIPlugin/Actions/ActionManagerComponent.h"
#include "UtilityAIPlugin/Public/UtilityAIController.h"

UActionAbstract::UActionAbstract()
{
	IsTickable = false;
	ActionState = EActionState::AS_Finished;
	IsIndividual = true;
	IsTimeLimited = false;
	IsStoppedWithUtilityChanged = true;
	ActionSlot = FName("Base");
	ActorsRolesNames.Add(FName("All"));
	ActionType = EActionTargetType::AT_Outer;
	Priority = 1;
	Cost = 1;
	Duration = 0.0f;
	CurrentDuration = 0.0f;
	IsChangingUrge = false;
	IsStoppedWithUtilityChanged = true;
	NewUrgeGrowthRate = 0.0f;
	SatisfyConditionType = EIntentionSatisfyCondition::SC_UtilityLevel;
	SatisfyUtilityLevel = 0.2f;
}

void UActionAbstract::Initialize_Implementation(APawn* owner, bool& isInitialized)
{
	ID = FGuid().NewGuid();
	OwnerPawn = owner;
	OwnerController = Cast<AUtilityAIController>(OwnerPawn->GetController());
	isInitialized = true;
}

void UActionAbstract::ExecutionCheck_Implementation(APawn* owner, EActionState& NewState)
{
	NewState = EActionState::AS_InProgress;
}

void UActionAbstract::CalculateUtility_Implementation(UIntentionAbstract* CurrentIntention, float& Utility)
{
	Utility = -1000.0f;
	if (IntentionsClasses.Contains(CurrentIntention->GetClass()))
	{
		if (ActorsRolesNames.Contains(FName("All")) || ActorsRolesNames.Contains(OwnerController->RoleName))
		{
			Utility = 1.0f / FMath::Max(Priority * 1.0f, 0.00001f);
		}
	}
}

void UActionAbstract::GetAllAreasOfInterest(TArray<AAreaOfInterest*>& AreasOfInterest)
{
	OwnerController->GetAllAreasOfInterest(AreasOfInterest);
}

void UActionAbstract::FindAreaOfInterest(FName AreaName, AAreaOfInterest*& AreaOfInterest)
{
	OwnerController->FindAreaOfInterest(AreaName, AreaOfInterest);
}

void UActionAbstract::GetAllPointsOfInterest(TArray<UPointOfInterestComponent*>& PointsOfInterest)
{
	OwnerController->GetAllPointsOfInterest(PointsOfInterest);
}


void UActionAbstract::FindPointOfInterest(FName PoIName, UPointOfInterestComponent*& PointOfInterest)
{
	OwnerController->FindPointOfInterest(PoIName, PointOfInterest);
}

void UActionAbstract::GetGameState(AGameStateBase*& GameState)
{
	OwnerController->GetGameState(GameState);
}

bool UActionAbstract::GetRandomPointInRadius(const FVector& Origin, float Radius, TSubclassOf<UNavigationQueryFilter> NavFilter, FVector& OutResult)
{
	OwnerController->GetRandomPointInRadius(Origin, Radius, NavFilter, OutResult);
	return true;
}

void UActionAbstract::ReceiveTick(float deltaTime)
{
	if (SatisfyConditionType == EIntentionSatisfyCondition::SC_UtilityLevel)
	{
		if (AffectedIntention->Utility <= SatisfyUtilityLevel)
			AffectedIntention->IsSatisfied = true;
	}
	if (IsTickable)
	{
		if (IsTimeLimited)
		{
			CurrentDuration += deltaTime;
			if (CurrentDuration >= Duration)
			{
				CurrentDuration = 0.0f;
				AffectedIntention->IsSatisfied = true;
				BreakAction();
			}
		}

		Tick(deltaTime);
	}
}

void UActionAbstract::StartExecution()
{
	UIntentionManagerComponent* imComp = Cast<UIntentionManagerComponent>(OwnerController->FindComponentByClass<class UIntentionManagerComponent>());
	AffectedIntention = imComp->GetCurrentIntention();

	if (SatisfyConditionType == EIntentionSatisfyCondition::SC_InitialSatisfied)
		AffectedIntention->IsSatisfied = true;
	if (IsStoppedWithUtilityChanged)
		OwnerController->StopMovement();
	ActionState = EActionState::AS_InProgress;
	if (IsChangingUrge)
	{
		bool isSuccess;
		imComp->ChangeIntentionGrowthRate(AffectedIntentionClass, NewUrgeGrowthRate, isSuccess);		
	}
	switch (ActionType)
	{
	case EActionTargetType::AT_Inner:
		OnStart();
		break;
	case EActionTargetType::AT_Outer:
		OwnerController->ChangeAIStage(EAIStage::AS_PoIChoosing);
		float maxUtility;
		FGuid maxTargetID;
		FindTarget(maxUtility, AffectedIntention, maxTargetID);
		ValidateTarget(maxTargetID);
		OnStart();
		break;
	}
}

void UActionAbstract::StopExecution()
{
	ActionState = EActionState::AS_Finished;
	CurrentDuration = 0.0f;
	if (IsChangingUrge)
	{
		UIntentionManagerComponent* imComp = Cast<UIntentionManagerComponent>(OwnerController->FindComponentByClass<class UIntentionManagerComponent>());
		bool isSuccess = false;
		imComp->RestoreIntentionGrowthRate(AffectedIntentionClass, true, isSuccess);
	}
	OnStop();
}

void UActionAbstract::BreakAction()
{
	StopExecution();
	UActionManagerComponent* amComp = Cast<UActionManagerComponent>(OwnerController->FindComponentByClass<class UActionManagerComponent>());
	if (amComp)
	{
		amComp->BreakActionByName(Name);
	}
}

void UActionAbstract::PauseExecution()
{
	ActionState = EActionState::AS_Paused;
}

void UActionAbstract::UnpauseExecution()
{
	ActionState = EActionState::AS_InProgress;
}

void UActionAbstract::FindTarget(float& maxUtility, UIntentionAbstract* intention, FGuid& maxID)
{
	maxUtility = -1000.0f;
	
	TArray<UPointOfInterestComponent*> pointsOfInterest = FUtilityAIPluginModule::Get().AIManager->GetPointsOfInterest();
	for (UPointOfInterestComponent* poi : pointsOfInterest)
	{
		if (!(poi->ParentArea->IsValidLowLevel()) || (poi->ParentArea == OwnerController->CurrentArea))
		{
			float poiUtility = -1000.0f;
			poi->CalculateUtility(OwnerPawn, intention, poiUtility);
			if (poiUtility > maxUtility)
			{
				maxUtility = poiUtility;
				maxID = poi->ID;
			}
		}
	}

	TArray<AAreaOfInterest*> areasOfInterest = FUtilityAIPluginModule::Get().AIManager->GetAreasOfInterest();
	for (AAreaOfInterest* area : areasOfInterest)
	{
		if (!(area == OwnerController->CurrentArea))
		{
			float areaUtility = -1000.0f;
			area->CalculateUtility(OwnerPawn, intention, areaUtility);
			if (areaUtility > maxUtility)
			{
				maxUtility = areaUtility;
				maxID = area->ID;
			}
		}
	}
}

void UActionAbstract::ValidateTarget(FGuid targetMaxID)
{
	if (targetMaxID.IsValid())
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(OwnerPawn->GetController());
		if (controller)
		{
			UPointOfInterestComponent* poi = FUtilityAIPluginModule::Get().AIManager->GetPointOfInterest(targetMaxID);
			if (poi)
			{

				TargetID = poi->ID;
				poi->AppointVisitor(OwnerPawn);
				controller->StartMovementToTarget(poi->GetOwner());
			}
			else
			{
				AAreaOfInterest* area = FUtilityAIPluginModule::Get().AIManager->GetAreaOfInterest(targetMaxID);
				if (area)
				{
					TargetID = area->ID;
					area->AppointUser(OwnerPawn);
					controller->StartMovementToTarget(area);
				}
			}
		}
	}
	else
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(OwnerPawn->GetController());
		UIntentionManagerComponent* intentionManager = controller->FindComponentByClass<UIntentionManagerComponent>();
		intentionManager->GetCurrentIntention()->Urge -= 0.5;
		intentionManager->MakeDecision();
		UE_LOG(LogTemp, Warning, TEXT("No suitable point of interest found!"));
	}
}



void UActionAbstract::OnStart_Implementation()
{


}

void UActionAbstract::OnStop_Implementation()
{

}



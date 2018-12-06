// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaOfInterest.h"

// TODO:
//      1) Collision as a component and change its shape in viewport (should be simple, need collision mesh variable and event to detect it's changing (but whats with mesh parameters?)
// Sets default values
AAreaOfInterest::AAreaOfInterest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	IsInitialized = false;
	CurrentUserNum = 0;
	IsLocalRolesAssigned = false;
	UtilityCalculationType = EAreaCalculationFunction::CF_MaxInnerPointsUtility;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxExtent = FVector(40.0f, 40.0f, 20.0f);
}

void AAreaOfInterest::Initialize_Implementation(bool& isSuccess)
{
	ID = FGuid().NewGuid();
	FUtilityAIPluginModule::Get().AIManager->AddArea(this);
	OnActorBeginOverlap.AddDynamic(this, &AAreaOfInterest::ProcessOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAreaOfInterest::EndOverlap);

	TArray<AActor*> pointsOfInterestActors;
	GetOverlappingActors(pointsOfInterestActors);

	for (AActor* actor : pointsOfInterestActors)
	{
		UPointOfInterestComponent* poi = actor->FindComponentByClass<UPointOfInterestComponent>();
		if (poi->IsValidLowLevel())
		{
			if (!poi->IsInitialized)
				poi->Initialize(isSuccess);
			if (poi->IsAttachedToArea)
			{
				PointsOfInterest.Add(poi->ID, poi);
				poi->ParentArea = this;
				//poi->SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}

	isSuccess = true;
}

void AAreaOfInterest::CalculateUtility(APawn* Pawn, UIntentionAbstract* IntentionObj, float& Utility)
{
	if (Pawn->IsValidLowLevelFast())
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
		FName userRole = FName(controller->RoleName);
		if (Roles.Contains(IntentionObj->GetClass()))
			controller->RoleName = *Roles.Find(IntentionObj->GetClass());

		switch (UtilityCalculationType)
		{
		case EAreaCalculationFunction::CF_MaxInnerPointsUtility:
		{
			Utility = -1000.0f;
			for (auto& Elem : PointsOfInterest)
			{
				float pointUtility;
				Elem.Value->CalculateUtility(Pawn, IntentionObj, pointUtility);
				Utility = FMath::Max(Utility, pointUtility);
			}
			break;
		}
		case EAreaCalculationFunction::CF_AverageInnerPointsUtility:
		{
			int n = 0;
			for (auto& Elem : PointsOfInterest)
			{
				float pointUtility;
				Elem.Value->CalculateUtility(Pawn, IntentionObj, pointUtility);
				if (pointUtility != -1000.0f)
				{
					n++;
					Utility += pointUtility;
				}
				Utility /= n;

			}
		}
		break;
		case EAreaCalculationFunction::CF_Custom:
			UtilityFunction(Pawn, Utility);
			break;
		default:
			break;
		}

		controller->RoleName = userRole;
	}
}

void AAreaOfInterest::AppointUserToPointOfInterest(FGuid UserID, UIntentionAbstract* Intention)
{
	if (Users.Contains(UserID))
	{
		APawn* user = *Users.Find(UserID);
		float choosenUtility = -1000.0f;
		FGuid choosenID;
		for (auto& Elem : PointsOfInterest)
		{
			float poiUtility;
			Elem.Value->CalculateUtility(user, Intention, poiUtility);
			if (poiUtility > choosenUtility)
			{
				choosenUtility = poiUtility;
				choosenID = Elem.Key;
			}
		}

		if (PointsOfInterest.Contains(choosenID))
		{
			AUtilityAIController* controller = Cast<AUtilityAIController>(user->GetController());
			if (controller->IsValidLowLevelFast())
			{
				UPointOfInterestComponent* poi = *PointsOfInterest.Find(choosenID);
				poi->AppointVisitor(user);
				controller->StartMovementToTarget(poi->GetOwner());
			}

		}
	}
}

void AAreaOfInterest::FindPointOfInterest(FGuid PointID, UPointOfInterestComponent*& poi)
{
	TArray<UPointOfInterestComponent*> points;
	PointsOfInterest.GenerateValueArray(points);
	for (UPointOfInterestComponent* point : points)
	{
		if (point->ID == PointID)
		{
			poi = point;
			return;
		}
	}
}

void AAreaOfInterest::FindPointOfInterestByName(FName PointName, UPointOfInterestComponent*& poi)
{
	TArray<UPointOfInterestComponent*> points;
	PointsOfInterest.GenerateValueArray(points);
	for (UPointOfInterestComponent* point : points)
	{
		if (point->Name == PointName)
		{
			poi = point;
			return;
		}
	}
}

void AAreaOfInterest::UtilityFunction_Implementation(APawn* Pawn, float& Utility)
{

}

void AAreaOfInterest::AppointUser(APawn* User)
{
	AUtilityAIController* controller = Cast<AUtilityAIController>(User->GetController());
	if (controller)
		AppointedUsers.Add(controller->ID);
	controller->OnPawnBeginDestroy.AddDynamic(this, &AAreaOfInterest::OnPawnBeginDestroy);
	
}

void AAreaOfInterest::OnPawnBeginDestroy(APawn* Pawn)
{
	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	if (AppointedUsers.Contains(controller->ID))
		AppointedUsers.Remove(controller->ID);

	if (Users.Contains(controller->ID))
	{
		Users.Remove(controller->ID);

		if (IsLocalRolesAssigned)
		{
			if (UsersRoles.Contains(controller->ID))
				UsersRoles.Remove(controller->ID);
		}
	}
	controller->OnPawnBeginDestroy.RemoveDynamic(this, &AAreaOfInterest::OnPawnBeginDestroy);
}

void AAreaOfInterest::DestroyArea()
{
	TArray<UPointOfInterestComponent*> points;
	PointsOfInterest.GenerateValueArray(points);
	for (UPointOfInterestComponent* point : points)
	{
		point->DestroyPoI(false);
	}

	TArray<APawn*> pawns;
	Users.GenerateValueArray(pawns);
	for (APawn* pawn : pawns)
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->GetController());
		controller->CurrentArea = NULL;
	}

	this->Destroy();
}

void AAreaOfInterest::ProcessOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APawn* pawn = Cast<APawn>(OtherActor);
	if (pawn->IsValidLowLevel())
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->GetController());
		if (controller->IsValidLowLevel())
		{
			controller->CurrentArea = this;
			if (controller->CurrentMovementTarget == this && controller->MovementType != EMovementType::MT_ToLocation)
			{
				//controller->StopMovement();
				UIntentionManagerComponent* intentionManager = controller->FindComponentByClass<class UIntentionManagerComponent>();
				if (intentionManager->IsValidLowLevel())
				{
					if (AppointedUsers.Contains(controller->ID))
						AppointedUsers.Remove(controller->ID);

					UIntentionAbstract* currentIntention = intentionManager->GetCurrentIntention();
					if (IsLocalRolesAssigned)
					{			
						if(Roles.Contains(currentIntention->GetClass()))
						{
							FName role = *Roles.Find(currentIntention->GetClass());
							controller->RoleName = role;
							UsersRoles.Add(controller->ID, role);
						}
						else
						{
							UsersRoles.Add(controller->ID, controller->RoleName);
						}
					}
					Users.Add(controller->ID, pawn);
					AppointUserToPointOfInterest(controller->ID, currentIntention);
				}
			}
		}
	}
}

void AAreaOfInterest::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APawn* pawn = Cast<APawn>(OtherActor);
	if (pawn->IsValidLowLevel())
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->GetController());
		if (controller->IsValidLowLevel())
		{
			if (AppointedUsers.Contains(controller->ID))
				AppointedUsers.Remove(controller->ID);

			if (Users.Contains(controller->ID))
			{
				Users.Remove(controller->ID);

				if (IsLocalRolesAssigned)
				{
					if (UsersRoles.Contains(controller->ID))
						UsersRoles.Remove(controller->ID);
					controller->RoleName = FName("Base");
				}
				controller->CurrentArea = NULL;

			}

			controller->OnPawnBeginDestroy.RemoveDynamic(this, &AAreaOfInterest::OnPawnBeginDestroy);

		}
	}
}

// Called when the game starts or when spawned
void AAreaOfInterest::BeginPlay()
{
	Super::BeginPlay();
	Initialize(IsInitialized);
}

void AAreaOfInterest::OnConstruction(const FTransform& Transform)
{
	BoxComponent->SetBoxExtent(BoxExtent);
}

// Called every frame
void AAreaOfInterest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


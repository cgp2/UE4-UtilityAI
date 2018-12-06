// Fill out your copyright notice in the Description page of Project Settings.

#include "PointOfInterestComponent.h"
#include "UtilityAIPlugin.h"
#include "UtilityAIPlugin/Public/UtilityAIController.h"
#include "CoreMinimal.h"

// Sets default values for this component's properties
UPointOfInterestComponent::UPointOfInterestComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaxUsers = 1;
	AcceptanceRadius = 200.0f;
	IsActivated = true;
	IsInitialized = false;
	IsOccupied = false;
	CalculationFunctionType = EPointUtilityCalculationFunction::CF_Distance;
	IsIntentionMinimaized = false;
	IsAttachedToArea = true;
	UsersRoles.Add(FName("All"));
	IsDistanceWithZ = false;
	IntentionSatisfyCondition = EPoIUtilitySatisfyCondition::SC_InitialSatisfied;
	/*SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("SphereCollision"));
	SphereCollision->InitSphereRadius(1.0f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereCollision->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);*/

	// ...
}


// Called when the game starts
void UPointOfInterestComponent::BeginPlay()
{
	Super::BeginPlay();
	Initialize(IsInitialized);
	// ...

}

void UPointOfInterestComponent::Initialize(bool& isSuccess)
{
	ID = FGuid().NewGuid();
	FUtilityAIPluginModule::Get().AIManager->AddPointOfInterest(this);

	isSuccess = true;
	OnPostInitialize.Broadcast();
}

void UPointOfInterestComponent::ProceedInteraction(APawn* Pawn)
{
	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	UIntentionManagerComponent* intentionComp = Cast<UIntentionManagerComponent>(controller->FindComponentByClass<class UIntentionManagerComponent>());
	if (controller->IsValidLowLevel())
	{
		if (AppointedUsers.Contains(controller->ID))
			AppointedUsers.Remove(controller->ID);

		intentionComp->IsBusy = true;

		if (!IsOccupied)
			StartInteraction(Pawn);
		else
			QueueUsers.Add(controller->ID, Pawn);
	}
}

void UPointOfInterestComponent::StartInteraction(APawn* Pawn)
{
	(Cast<AUtilityAIController>(Pawn->GetController()))->ChangeAIStage(EAIStage::AS_Interaction);
	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	UIntentionManagerComponent* intentionComp = Cast<UIntentionManagerComponent>(controller->FindComponentByClass<class UIntentionManagerComponent>());
	if (QueueUsers.Contains(controller->ID))
		QueueUsers.Remove(controller->ID);

	if (intentionComp->IsValidLowLevel())
	{
		Users.Add(controller->ID, Pawn);
		UsersInteractionTime.Add(controller->ID, 0.0f);
		UsersIntentionsSatisfied.Add(controller->ID, false);

		if (Users.Num() >= MaxUsers)
			IsOccupied = true;

		//Changing growth rate of current intention to satisfy it
		UIntentionAbstract* currentIntention = intentionComp->GetCurrentIntention();
		;
		if (SatisfiedIntentions.Contains(currentIntention->GetClass()))
		{
			currentIntention->ChangeGrowthRate(*SatisfiedIntentions.Find(currentIntention->GetClass()));

			//Save current intention id 
			UsersIntentions.Add(controller->ID, currentIntention->GetClass());

			if (IntentionSatisfyCondition == EPoIUtilitySatisfyCondition::SC_InitialSatisfied)
				currentIntention->IsSatisfied = true;

			OnStartInteraction.Broadcast(Pawn);
		}
		else
		{
			FinishInteraction(Pawn);
		}
	}
}

void UPointOfInterestComponent::UtilityFunction_Implementation(APawn* Pawn, float& Utility)
{

}

void UPointOfInterestComponent::FinishInteraction(APawn* Pawn)
{
	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	UIntentionManagerComponent* intentionComp = Cast<UIntentionManagerComponent>(controller->FindComponentByClass<class UIntentionManagerComponent>());

	if (Users.Contains(controller->ID))
		Users.Remove(controller->ID);

	if (QueueUsers.Contains(controller->ID))
		QueueUsers.Remove(controller->ID);

	if (UsersInteractionTime.Contains(controller->ID))
		UsersInteractionTime.Remove(controller->ID);

	if (UsersIntentionsSatisfied.Contains(controller->ID))
		UsersIntentionsSatisfied.Remove(controller->ID);

	TSubclassOf<UIntentionAbstract> intentionClass;
	if (UsersIntentions.Contains(controller->ID))
	{
		intentionClass = *UsersIntentions.Find(controller->ID);
		UsersIntentions.Remove(controller->ID);
	}

	if (!intentionComp->IsPendingKill() && intentionClass->IsValidLowLevelFast())
	{
		intentionComp->IsBusy = false;
		bool isSuccess = false;
		intentionComp->RestoreIntentionGrowthRate(intentionClass, true, isSuccess);
		OnEndInteraction.Broadcast(Pawn);
		controller->OnPawnBeginDestroy.RemoveDynamic(this, &UPointOfInterestComponent::OnPawnBeginDestroy);
	}

	if (Users.Num() < MaxUsers)
	{
		IsOccupied = false;

		if (QueueUsers.Num() > 0)
		{
			TArray<APawn*> queueUsers;
			QueueUsers.GenerateValueArray(queueUsers);
			StartInteraction(queueUsers[0]);
		}
	}
}

void UPointOfInterestComponent::CalculateUtility(APawn* Pawn, UIntentionAbstract* IntentionObj, float& Utility)
{
	Utility = -1000;
	if (!IsActivated)
		return;

	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	UIntentionManagerComponent* intentionComp = Pawn->GetController()->FindComponentByClass<class UIntentionManagerComponent>();
	if (!controller->IsValidLowLevel())
		return;
	if (!intentionComp)
		return;

	if (UsersRoles.Contains(FName("All")) || UsersRoles.Contains(controller->RoleName))
	{
		UIntentionAbstract* currentIntention = intentionComp->GetCurrentIntention();

		if (!currentIntention->IsValidLowLevel())
			return;

		//If intention is targeted
		if (currentIntention->IsTargeted)
		{
			if (currentIntention->TargetName == Name)
			{
				Utility = 0.7f;
				return;
			}
			return;
		}

		if (CheckIntentionContainment(currentIntention->GetClass()))
		{
			float distance;
			if (IsDistanceWithZ)
			{
				distance = (Pawn->GetActorLocation() - GetOwner()->GetActorLocation()).Size();
			}
			else
			{
				FVector pawnLocation = Pawn->GetActorLocation();
				FVector poiLocation = GetOwner()->GetActorLocation();
				pawnLocation.Z = 0.0f;
				poiLocation.Z = 0.0f;
				distance = (pawnLocation - poiLocation).Size();
			}

			switch (CalculationFunctionType)
			{
			case EPointUtilityCalculationFunction::CF_Distance:
			{
				Utility = 1.0f / distance;
				break;
			}
			case EPointUtilityCalculationFunction::CF_Random:
			{
				Utility = FMath::RandRange(0.1f, 0.7f);
				break;
			}
			case EPointUtilityCalculationFunction::CF_RandomDistance:
			{
				Utility = 1.0f / distance * FMath::RandRange(0.5f, 1.5f);
				break;
			}
			case EPointUtilityCalculationFunction::CF_CapacityDistance:
			{
				//Some great math by genius programmer :)
				float capasityUt = 0.007 * FMath::LogX(1.75f, FMath::Max((MaxUsers - Users.Num() + 1.0f), 0.0001f));
				Utility = 1.0f / FMath::Max(distance, 0.0001f) * capasityUt;
				break;
			}
			case EPointUtilityCalculationFunction::CF_OccupancyRandom:
			{
				if (AppointedUsers.Num() + QueueUsers.Num() + Users.Num() < MaxUsers)
					Utility = FMath::RandRange(0.1f, 0.7f);
				break;
			}
			case EPointUtilityCalculationFunction::CF_Custom:
				UtilityFunction(Pawn, Utility);
				break;
			default:
				break;
			}

		}
	}
}

void UPointOfInterestComponent::OnPawnBeginDestroy(APawn* Pawn)
{
	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	UIntentionManagerComponent* intentionComp = Cast<UIntentionManagerComponent>(controller->FindComponentByClass<class UIntentionManagerComponent>());

	if (Users.Contains(controller->ID))
		Users.Remove(controller->ID);

	if (QueueUsers.Contains(controller->ID))
		QueueUsers.Remove(controller->ID);

	if (UsersInteractionTime.Contains(controller->ID))
		UsersInteractionTime.Remove(controller->ID);

	if (UsersIntentionsSatisfied.Contains(controller->ID))
		UsersIntentionsSatisfied.Remove(controller->ID);

	TSubclassOf<UIntentionAbstract> intentionClass;
	if (UsersIntentions.Contains(controller->ID))
	{
		intentionClass = *UsersIntentions.Find(controller->ID);
		UsersIntentions.Remove(controller->ID);
	}

	controller->OnPawnBeginDestroy.RemoveDynamic(this, &UPointOfInterestComponent::OnPawnBeginDestroy);

	if (Users.Num() < MaxUsers)
	{
		IsOccupied = false;

		if (QueueUsers.Num() > 0)
		{
			TArray<APawn*> queueUsers;
			QueueUsers.GenerateValueArray(queueUsers);
			StartInteraction(queueUsers[0]);
		}
	}
}

void UPointOfInterestComponent::DestroyPoI(bool isParentActorDestroyed /*= true*/)
{
	IsActivated = false;
	for (auto& Elem : Users)
	{
		FinishInteraction(Elem.Value);
		//What if is intention is not satisfied??
	}

	for (auto& Elem : QueueUsers)
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(Elem.Value->GetController());
		FinishInteraction(Elem.Value);
		controller->Reappoint();
	}

	for (auto& Elem : AppointedUsers)
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(Elem.Value->GetController());
		controller->Reappoint();
	}

	if (ParentArea->IsValidLowLevel())
	{
		ParentArea->PointsOfInterest.Remove(ID);
	}

	this->DestroyComponent();
	if (isParentActorDestroyed)
	{
		GetOwner()->Destroy();
	}
}

void UPointOfInterestComponent::AppointVisitor(APawn* Pawn)
{
	AUtilityAIController* controller = Cast<AUtilityAIController>(Pawn->GetController());
	AppointedUsers.Add(controller->ID, Pawn);
}

bool UPointOfInterestComponent::CheckIntentionContainment(TSubclassOf<UIntentionAbstract> intentionClass)
{
	TArray<TSubclassOf<UIntentionAbstract>> intentionsClasses;
	SatisfiedIntentions.GetKeys(intentionsClasses);

	return intentionsClasses.Contains(intentionClass);
}

// Called every frame
void UPointOfInterestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& Elem : UsersInteractionTime)
	{
		Elem.Value += DeltaTime;
		if (IntentionSatisfyCondition == EPoIUtilitySatisfyCondition::SC_InteractionTime)
		{
			bool* isSatisfied = UsersIntentionsSatisfied.Find(Elem.Key);
			if (!(*isSatisfied))
			{
				if (Elem.Value >= MinInteractionTime)
				{

					TSubclassOf<UIntentionAbstract>* intentionClass = (UsersIntentions.Find(Elem.Key));
					APawn** pawn = Users.Find(Elem.Key);
					UIntentionManagerComponent* intentionManager = ((*pawn)->GetController())->FindComponentByClass<UIntentionManagerComponent>();
					UIntentionAbstract* intentionObj;
					intentionManager->GetIntentionByClass(*intentionClass, intentionObj);
					intentionObj->IsSatisfied = true;
					UsersIntentionsSatisfied.Emplace(Elem.Key, true);

				}
			}
		}
	}

	if (IntentionSatisfyCondition == EPoIUtilitySatisfyCondition::SC_UtilityLevel)
	{
		for (auto& Elem : Users)
		{
			TSubclassOf<UIntentionAbstract>* intentionClass = (UsersIntentions.Find(Elem.Key));
			UIntentionManagerComponent* intentionManager = (Elem.Value->GetController())->FindComponentByClass<UIntentionManagerComponent>();
			UIntentionAbstract* intentionObj;
			intentionManager->GetIntentionByClass(*intentionClass, intentionObj);
			if (intentionObj->Utility <= SatisfyUtilityLevel)
				intentionObj->IsSatisfied = true;
		}
	}
	// ...
}


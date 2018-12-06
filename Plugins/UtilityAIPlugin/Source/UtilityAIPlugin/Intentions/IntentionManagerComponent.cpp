// Fill out your copyright notice in the Description page of Project Settings.

#include "IntentionManagerComponent.h"
#include "UtilityAIPlugin.h"
#include "Engine/World.h"
#include "UtilityAIPlugin/Public/UtilityAIController.h"
#include "Runtime/Engine/Public/TimerManager.h"

// Sets default values for this component's properties
UIntentionManagerComponent::UIntentionManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	TickTime = 0.5f;
	IsPaused = false;
	IsBusy = false;
	isValidChoise = false;
	pauseTime = 0.0f;
	IntentionsUpdateTime = 0.5f;
	DeceisionMakingTime = 2.0f;

	// ...
}

void UIntentionManagerComponent::GetOwnerPawn(AActor*& OwnerPawn)
{
	OwnerPawn = (Cast<AUtilityAIController>(GetOwner())->GetPawn());
}

APawn* UIntentionManagerComponent::GetPawn()
{
	return (Cast<AUtilityAIController>(GetOwner())->GetPawn());
}

// Called when the game starts
void UIntentionManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...

}

void UIntentionManagerComponent::Initialize_Implementation(bool& isInitialized)
{
	FUtilityAIPluginModule::Get().AIManager->AddAIPawn(GetPawn());

	for (TSubclassOf<UIntentionAbstract> intentionClass : IntentionClasses)
	{
		if (intentionClass->IsValidLowLevelFast())
		{
			bool isIntentionInitialized = true;
			UIntentionAbstract *tempIntention = NewObject<UIntentionAbstract>(this, intentionClass);
			tempIntention->Initialize(GetPawn(), isIntentionInitialized);
			if (isIntentionInitialized)
				Intentions.Add(tempIntention->ID, tempIntention);
		}
	}

	isInitialized = true;
	//UpdateIntentions();
	/*ChooseIntention();*/
}

void UIntentionManagerComponent::PostInitialize_Implementation(bool& isInitialized)
{
	GetWorld()->GetTimerManager().SetTimer(UpdateIntentionsTimerHandler, this, &UIntentionManagerComponent::UpdateIntentions, IntentionsUpdateTime, true);
	GetWorld()->GetTimerManager().SetTimer(ChooseIntentionTimerHandler, this, &UIntentionManagerComponent::ChooseIntention, DeceisionMakingTime, true);
	UpdateIntentions();
}

void UIntentionManagerComponent::AddIntentionByClass(TSubclassOf<UIntentionAbstract> IntentionClass, UIntentionAbstract*& AddedIntention)
{
	/*auto utObj = NewObject<UUtilityAbstract>(this, UtilityClass->GetFName(), RF_NoFlags, UtilityClass->GetDefaultObject());*/
	if (IntentionClass->IsValidLowLevel())
	{
		bool isInitialized = false;
		UIntentionAbstract *tempIntention = NewObject<UIntentionAbstract>(this, IntentionClass);
		tempIntention->Initialize(GetPawn(), isInitialized);
		/*AddedUtility = NewObject<UUtilityAbstract>(this, UtilityClass);*/
		if (isInitialized)
		{
			IntentionClasses.Add(IntentionClass);
			Intentions.Add(tempIntention->ID, tempIntention);
			AddedIntention = tempIntention;
		}
	}
}

void UIntentionManagerComponent::AddIntention(UIntentionAbstract* Intention)
{
	if (Intention->IsValidLowLevelFast())
	{
		bool isinitialized = false;
		Intention->Initialize(GetPawn(), isinitialized);
		if (isinitialized)
		{
			Intentions.Add(Intention->ID, Intention);
			IntentionClasses.Add(Intention->GetClass());
		}
	}
}

void UIntentionManagerComponent::RemoveIntentionByClass(const TSubclassOf<UIntentionAbstract> IntentionClass, bool& isSuccess)
{
	if (IntentionClass->IsValidLowLevelFast())
	{
		int32 isFound = IntentionClasses.Remove(IntentionClass);
		isSuccess = false;
		if (isFound != -1)
		{
			isSuccess = true;
			FGuid utID = GetIntentionIDByClass(IntentionClass);
			Intentions.Remove(utID);
		}
	}
}

void UIntentionManagerComponent::MinimizeIntention(const TSubclassOf<UIntentionAbstract> IntentionClass, const bool MinimaizeGrowth, float& NewUtility)
{
	if (IntentionClass->IsValidLowLevel())
	{
		FGuid UtID = GetIntentionIDByClass(IntentionClass);
		UIntentionAbstract** utilityObject = Intentions.Find(UtID);
		(*utilityObject)->MinimizeUtility(MinimaizeGrowth, NewUtility);
	}

}

void UIntentionManagerComponent::MaximizeIntention(const TSubclassOf<UIntentionAbstract> IntentionClass, float& NewUtility)
{
	if (IntentionClass->IsValidLowLevel())
	{
		FGuid UtID = GetIntentionIDByClass(IntentionClass);
		UIntentionAbstract** utilityObject = Intentions.Find(UtID);
		(*utilityObject)->MaximizeUtility(NewUtility);
	}
}

void UIntentionManagerComponent::RandomizeIntention(const TSubclassOf<UIntentionAbstract> IntentionClass, float& NewUtility)
{
	if (IntentionClass->IsValidLowLevel())
	{
		FGuid UtID = GetIntentionIDByClass(IntentionClass);
		UIntentionAbstract** intentionObject = Intentions.Find(UtID);
		(*intentionObject)->Randomize(NewUtility);
	}
}

void UIntentionManagerComponent::UnpauseUpdate_Implementation()
{
	IsPaused = false;
	pauseTime = 0.0f;
	pauseDuration = 0.0f;
}

void UIntentionManagerComponent::PauseUpdate_Implementation(float duration /*= -1.0f*/)
{
	IsPaused = true;
	if (duration != -1)
	{
		pauseDuration = duration;
		pauseTime = 0.0f;
	}
}

void UIntentionManagerComponent::UpdateIntentions()
{
	if (!Cast<AUtilityAIController>(GetOwner())->IsInitialized)
		return;

	TArray<UIntentionAbstract*> values;
	Intentions.GenerateValueArray(values);
	for (int32 i = 0; i < values.Num(); i++)
	{
		values[i]->ExecuteTick(TickTime);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(values.Num()));
	}

}

void UIntentionManagerComponent::ChooseIntention()
{
	if (!Cast<AUtilityAIController>(GetOwner())->IsInitialized)
		return;

	(Cast<AUtilityAIController>(GetOwner()))->ChangeAIStage(EAIStage::AS_IntentionChoosing);
	if (!IsPaused)
	{
		if (CurrentIntentionID.IsValid())
		{
			UIntentionAbstract* currentIntentionObj = *Intentions.Find(CurrentIntentionID);
			if (currentIntentionObj == nullptr)
			{
				return;
			}

			if ((currentIntentionObj)->IsInteruptable && currentIntentionObj->IsSatisfied)
			{
				return;
			}

			float maxUtility = -1.0f;
			UIntentionAbstract* maxIntentionObj = nullptr;
			for (auto& elem : Intentions)
			{
				if (elem.Value->Utility > maxUtility)
				{
					maxUtility = elem.Value->Utility;
					maxIntentionObj = elem.Value;
				}
			}

			if (currentIntentionObj->Utility < maxUtility)
			{
				ValidateChoice(maxIntentionObj);
				if (isValidChoise)
				{
					(currentIntentionObj)->OnDetach();
					PreviousIntentionID = CurrentIntentionID;
					CurrentIntentionID = maxIntentionObj->ID;
					maxIntentionObj->IsSatisfied = false;
					maxIntentionObj->OnApply();
					OnIntentionChanged.Broadcast(maxIntentionObj);
				}
			}
		}
		else
		{
			float maxUtility = -1.0f;
			UIntentionAbstract* maxIntentionObj = nullptr;
			for (auto& elem : Intentions)
			{
				if (elem.Value->Utility > maxUtility)
				{
					maxUtility = elem.Value->Utility;
					maxIntentionObj = elem.Value;
				}
			}

			ValidateChoice(maxIntentionObj);
			if (!isValidChoise)
			{
				return;
			}

			PreviousIntentionID = CurrentIntentionID;
			CurrentIntentionID = maxIntentionObj->ID;
			maxIntentionObj->IsSatisfied = false;
			maxIntentionObj->OnApply();
			OnIntentionChanged.Broadcast(maxIntentionObj);
		}
	}
}

void UIntentionManagerComponent::GetCurrentIntentionID(FGuid& CurIntentionID)
{
	CurIntentionID = CurrentIntentionID;
}

void UIntentionManagerComponent::GetPreviousIntentionID(FGuid& PrevIntentionID)
{
	PrevIntentionID = PreviousIntentionID;
}

FGuid UIntentionManagerComponent::GetIntentionIDByClass(const TSubclassOf<UIntentionAbstract> IntentionClass)
{
	if (!IntentionClass->IsValidLowLevel())
	{
		return FGuid();
	}

	TArray<UIntentionAbstract*> values;
	Intentions.GenerateValueArray(values);
	for (int32 i = 0; i < values.Num(); i++)
	{
		if (values[i]->GetClass() == IntentionClass)
		{
			TArray<FGuid> keys;
			Intentions.GetKeys(keys);
			return keys[i];
		}
	}
	return FGuid();
}

// Called every frame
void UIntentionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsPaused)
	{
		pauseTime += DeltaTime;
		if (pauseTime >= pauseDuration)
		{
			UnpauseUpdate();
		}
	}
	// ...
}

void UIntentionManagerComponent::ValidateChoice_Implementation(UIntentionAbstract* MaxIntentionObj)
{
	if (MaxIntentionObj != nullptr)
	{
		isValidChoise = MaxIntentionObj->ID != CurrentIntentionID;
		return;
	}
	isValidChoise = false;
}

UIntentionAbstract* UIntentionManagerComponent::GetCurrentIntention()
{
	if (Intentions.Contains(CurrentIntentionID))
	{
		UIntentionAbstract** intentionObject = Intentions.Find(CurrentIntentionID);
		return *intentionObject;
	}

	return NULL;

}

UIntentionAbstract* UIntentionManagerComponent::GetPreviousIntention()
{
	if (Intentions.Contains(PreviousIntentionID))
	{
		UIntentionAbstract** intentionObject = Intentions.Find(PreviousIntentionID);
		return *intentionObject;
	}

	return nullptr;
}

void UIntentionManagerComponent::GetIntentionByID(FGuid IntentionID, UIntentionAbstract *&Intention)
{
	if (IntentionID.IsValid())
	{
		for (auto& Elem : Intentions)
		{
			if (Elem.Value->ID == IntentionID)
			{
				Intention = Elem.Value;
				return;
			}
		}
	}
	Intention = NULL;
}

void UIntentionManagerComponent::GetIntentionByClass(const TSubclassOf<UIntentionAbstract> IntentionClass, UIntentionAbstract *&Intention)
{
	if (IntentionClass->IsValidLowLevel())
	{
		for (auto& Elem : Intentions)
		{
			if (Elem.Value->GetClass() == IntentionClass)
			{
				Intention = Elem.Value;
				return;
			}
		}
	}
	Intention = NULL;
}

void UIntentionManagerComponent::GetIntentionsByGroupName(FName GroupName, TArray<UIntentionAbstract*>& IntentionArray)
{
	TArray<UIntentionAbstract*> res;
	if (GroupName.IsValid())
	{
		for (auto& Elem : Intentions)
		{
			if (Elem.Value->Group == GroupName)
			{
				res.Add(Elem.Value);
			}
		}
	}
	IntentionArray = res;
}

void UIntentionManagerComponent::MinimizeCurrentIntention(const bool MinimaizeGrowth, float& NewUtility)
{
	UIntentionAbstract** intentionObject = Intentions.Find(CurrentIntentionID);
	(*intentionObject)->MinimizeUtility(MinimaizeGrowth, NewUtility);
}

void UIntentionManagerComponent::MaximizeCurrentUtility(float& NewUtility)
{
	UIntentionAbstract** intentionObject = Intentions.Find(CurrentIntentionID);
	(*intentionObject)->MaximizeUtility(NewUtility);
}

void UIntentionManagerComponent::RestoreIntentionGrowthRate(const TSubclassOf<UIntentionAbstract> IntentionClass, bool RestoreInitial, bool &IsSuccess)
{
	if (IntentionClass->IsValidLowLevelFast())
	{
		IsSuccess = IntentionClasses.Contains(IntentionClass);
		if (IsSuccess)
		{
			FGuid IntentionID = GetIntentionIDByClass(IntentionClass);
			UIntentionAbstract* intentionObject = *Intentions.Find(IntentionID);
			float growthRate;
			if (RestoreInitial)
			{
				intentionObject->RestoreInitialGrowthRate(growthRate);
			}
			else
			{
				intentionObject->RestoreLastGrowthRate(growthRate);
			}
		}
	}
}

void UIntentionManagerComponent::ChangeIntentionGrowthRate(const TSubclassOf<UIntentionAbstract> IntentionClass, float NewGrowthRate, bool &IsSuccess)
{
	if (IntentionClass->IsValidLowLevelFast())
	{
		IsSuccess = IntentionClasses.Contains(IntentionClass);
		if (IsSuccess)
		{
			FGuid IntentionID = GetIntentionIDByClass(IntentionClass);
			UIntentionAbstract* intentionObject = *Intentions.Find(IntentionID);
			intentionObject->ChangeGrowthRate(NewGrowthRate);
		}
	}
}

void UIntentionManagerComponent::ChangeCurrentIntentionGrowthRate(const float NewGrowthRate)
{
	UIntentionAbstract* currentIntention;
	GetIntentionByID(CurrentIntentionID, currentIntention);
	if (currentIntention)
		currentIntention->ChangeGrowthRate(NewGrowthRate);
}

void UIntentionManagerComponent::MakeDecision()
{
	UpdateIntentions();
	ChooseIntention();
}

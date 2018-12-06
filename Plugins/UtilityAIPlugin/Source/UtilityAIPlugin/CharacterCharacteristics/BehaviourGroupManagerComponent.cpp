// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviourGroupManagerComponent.h"

// Sets default values for this component's properties
UBehaviourGroupManagerComponent::UBehaviourGroupManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features	
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	BehaviourGroupClass = NULL;


	// ...
}

void UBehaviourGroupManagerComponent::PostInitialize_Implementation(bool& IsSuccess)
{
	AUtilityAIController* owner = Cast<AUtilityAIController>(GetOwner());
	UIntentionManagerComponent* intManager = owner->FindComponentByClass<UIntentionManagerComponent>();
	UActionManagerComponent* actManager = owner->FindComponentByClass<UActionManagerComponent>();

	if (BehaviorGroupType == EBehaviorGroupType::BG_Characteristics)
	{
		for (auto& elem : CharacteristicsClasses)
		{
			FCharacteristicParametrs tempStr = elem.Value;
			UCharacteristicBase *tempChrctr = NewObject<UCharacteristicBase>(this, elem.Key);
			tempChrctr->Value = tempStr.Value;
			tempChrctr->Multiplier = tempStr.Mult;
			tempChrctr->UpdateInfluence(intManager, actManager);
			Characteristics.Add(tempChrctr);
		}
	}
	IsSuccess = true;
}



void UBehaviourGroupManagerComponent::Initialize_Implementation(bool& isInitialized)
{
	if (BehaviourGroupClass->IsValidLowLevel())
	{
		BehaviourGroupObj = NewObject<UBehaviorGroupAbstract>(this, BehaviourGroupClass);
		AUtilityAIController* owner = Cast<AUtilityAIController>(GetOwner());
		UIntentionManagerComponent* intManager = owner->FindComponentByClass<UIntentionManagerComponent>();
		UActionManagerComponent* actManager = owner->FindComponentByClass<UActionManagerComponent>();

		intManager->IntentionClasses.Insert(BehaviourGroupObj->InitialIntentions, 0);
		actManager->ActionClasses.Insert(BehaviourGroupObj->InitialActions, 0);
		owner->RoleName = BehaviourGroupObj->DefaultRoleName;

		isInitialized = true;
	}
	else
	{
		isInitialized = false;
	}
}

// Called when the game starts
void  UBehaviourGroupManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void  UBehaviourGroupManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBehaviourGroupManagerComponent::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (e.GetPropertyName() == FName("BehaviorGroupType"))
	{
		switch (BehaviorGroupType)
		{
		case EBehaviorGroupType::BG_Default:
			IsCharacteristics = false;
			break;
		case EBehaviorGroupType::BG_Characteristics:
			IsCharacteristics = true;
			break;
		default:
			break;
		}
	}
	//else if (e.GetPropertyName() == FName("CharacteristicsClasses"))
	//{
	//	if(e.ChangeType == EPropertyChangeType::ArrayAdd || e.ChangeType == EPropertyChangeType::ArrayClear || e.ChangeType == EPropertyChangeType::ValueSet)
	//	{
	//		Characteristics.Empty();
	//		for (auto& elem : CharacteristicsClasses)
	//		{
	//			if(elem.Key->IsValidLowLevel())
	//			{
	//				if(!elem.Value.isDirty)
	//				{
	//					UCharacteristicBase *tempChrctr = NewObject<UCharacteristicBase>(this, elem.Key);
	//					Characteristics.Add(tempChrctr);
	//					elem.Value.Value = tempChrctr->Value;
	//					elem.Value.Mult = tempChrctr->Multiplier;
	//				}
	//			}
	//			if (e.ChangeType == EPropertyChangeType::ArrayAdd)
	//				DirtyMap.Add(elem.Key, false);
	//		}
	//		
	//	}
	//}
	//else if (e.GetPropertyName() == FName("Mult") || e.GetPropertyName() == FName("Value"))
	//{

	//	//UStruct* strct = e.Property->GetOwnerStruct();
	//	//TArray<FCharacteristicParametrs> structs;
	//	//CharacteristicsClasses.GenerateValueArray(structs);
	//	//for (auto& kek : structs)
	//	//{
	//	//	strct->Be
	//	//}
	//	//bool b = structs.Contains(strct);


	//	//UBoolProperty* f = Cast<UBoolProperty>(strct->FindPropertyByName(FName("isDirty")));
	//	//int32 index = f->GetOffset_ForInternal();
	//	////f->SetPropertyValue_InContainer(strct, true, index);
	//}
}


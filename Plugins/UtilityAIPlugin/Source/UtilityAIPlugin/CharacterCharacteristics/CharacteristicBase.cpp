// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacteristicBase.h"

UCharacteristicBase::UCharacteristicBase()
{
	Name = FName("Default");
	Value = 0;
	Multiplier = 0.5f;
	InfluensedGroup = FName("Base");
}

void UCharacteristicBase::UpdateInfluence(UIntentionManagerComponent* IntentionManager, UActionManagerComponent* ActionManager)
{
	TArray<UIntentionAbstract*> intentions;
	IntentionManager->GetIntentionsByGroupName(InfluensedGroup, intentions);

	if (intentions.Num() > 0)
	{
		for (UIntentionAbstract* intention : intentions)
		{
			intention->initialGrowthRate += Multiplier * 1.0f / FMath::Max(0.00001f, (float)Value);
			intention->GrowthRate = intention->initialGrowthRate;
			UE_LOG(LogTemp, Warning, TEXT("%f"), intention->GrowthRate);
		}
	}
}

void UCharacteristicBase::OnApply_Implementation(UIntentionManagerComponent* IntentionManager, UActionManagerComponent* ActionManager)
{

}


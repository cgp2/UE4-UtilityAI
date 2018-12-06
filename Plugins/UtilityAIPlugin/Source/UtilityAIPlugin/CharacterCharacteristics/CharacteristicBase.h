// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilityAIPlugin/Public/AIActorsManager.h"
#include "CharacteristicBase.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = UtilityAI)
class UTILITYAIPLUGIN_API UCharacteristicBase : public UObject
{
	GENERATED_BODY()

public:

	UCharacteristicBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		FName Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base", meta = (UIMin = 0, UIMax = 10))
		int32 Value;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base", meta = (UIMin = 0.0f, UIMax = 1.0f))
		float Multiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		FName InfluensedGroup;

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Events")
		void UpdateInfluence(UIntentionManagerComponent* IntentionManager, UActionManagerComponent* ActionManager);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnApply(UIntentionManagerComponent* IntentionManager, UActionManagerComponent* ActionManager);
};

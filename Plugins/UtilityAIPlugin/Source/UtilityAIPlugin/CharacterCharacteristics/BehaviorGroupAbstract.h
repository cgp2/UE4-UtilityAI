// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UtilityAIPlugin//Public/AIActorsManager.h"
#include "BehaviorGroupAbstract.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = UtilityAI)
class UTILITYAIPLUGIN_API UBehaviorGroupAbstract : public UObject
{
	GENERATED_BODY()

public:
	UBehaviorGroupAbstract();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		FName Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		FName DefaultRoleName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		TArray<TSubclassOf<UIntentionAbstract>> InitialIntentions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		TArray<TSubclassOf<UActionAbstract>> InitialActions;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"), Category = "Base")
		UIntentionManagerComponent* IntentionManagerRef;
		
};

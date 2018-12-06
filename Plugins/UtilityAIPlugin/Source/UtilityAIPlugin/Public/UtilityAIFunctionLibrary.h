// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityAIPlugin//Public/UtilityAIController.h"
#include "UtilityAIFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class UTILITYAIPLUGIN_API UUtilityAIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UtilityAI", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool SpawnAIPawn(UObject* WorldContextObject, TSubclassOf<APawn> PawnClass, TSubclassOf<AUtilityAIController> Controller, TSubclassOf<UBehaviorGroupAbstract> BehaviorGroup,
								FTransform Transform, APawn*& SpawndedPawn);

	UFUNCTION(BlueprintCallable, Category = "UtilityAI", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool DestroyAIPawn(UObject* WorldContextObject, APawn* pawn);

	UFUNCTION(BlueprintCallable, Category = "UtilityAI", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool DestroyPointOfInterest(UObject* WorldContextObject, UPointOfInterestComponent* poi, bool DestroyParentActor);

	UFUNCTION(BlueprintCallable, Category = "UtilityAI", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool DestroyAreaOfInterest(UObject* WorldContextObject, AAreaOfInterest* area);

	UFUNCTION(BlueprintCallable, Category = "UtilityAI")
		static void StartAIDebugMode();

	UFUNCTION(BlueprintCallable, Category = "UtilityAI")
		static void EndAIDebugMode();
};

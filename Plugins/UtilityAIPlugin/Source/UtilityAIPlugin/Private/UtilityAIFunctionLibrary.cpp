// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilityAIFunctionLibrary.h"
#include "UtilityAIPlugin/Public/UtilityAIPlugin.h"
#include "GameFramework/Pawn.h"
#include "EngineMinimal.h"

//APawn* UUtilityAIFunctionLibrary::SpawnAIPawn(UObject* WorldContextObject, TSubclassOf<APawn> PawnClass, TSubclassOf<AUtilityAIController> Controller, TSubclassOf<UBehaviorGroupAbstract> BehaviorGroup, FTransform Transform)
//{
//	
//}

bool UUtilityAIFunctionLibrary::SpawnAIPawn(UObject* WorldContextObject, TSubclassOf<APawn> PawnClass, TSubclassOf<AUtilityAIController> Controller, TSubclassOf<UBehaviorGroupAbstract> BehaviorGroup, 
											FTransform Transform, APawn*& SpawndedPawn)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	APawn* pawn = Cast<APawn>(World->SpawnActor(PawnClass));
	pawn->SetActorTransform(Transform);
	pawn->AIControllerClass = Controller;
	pawn->SpawnDefaultController();
	AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->Controller);
	controller->BehaviorGroupManager->BehaviourGroupClass = BehaviorGroup;
	pawn->Controller->SetPawn(pawn);

	bool isSuccess;
	Cast<AUtilityAIController>(pawn->GetController())->Initialize(isSuccess);
	
	SpawndedPawn = pawn;

	return isSuccess;
}

bool UUtilityAIFunctionLibrary::DestroyAIPawn(UObject* WorldContextObject, APawn* pawn)
{
	if (pawn->IsValidLowLevel())
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);

		AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->GetController());
		controller->OnPawnBeginDestroy.Broadcast(pawn);
		FUtilityAIPluginModule::Get().AIManager->RemoveAIPawn(pawn);
		pawn->Destroy();
	}
	return true;
}

bool UUtilityAIFunctionLibrary::DestroyPointOfInterest(UObject* WorldContextObject, UPointOfInterestComponent* poi, bool DestroyParentActor)
{
	if (poi->IsValidLowLevel())
	{
		FUtilityAIPluginModule::Get().AIManager->RemovePointOfInterest(poi);
		poi->DestroyPoI(DestroyParentActor);
	}
	return true;
}

bool UUtilityAIFunctionLibrary::DestroyAreaOfInterest(UObject* WorldContextObject, AAreaOfInterest* area)
{
	if (area->IsValidLowLevel())
	{
		FUtilityAIPluginModule::Get().AIManager->RemoveAreaOfInterest(area);
		area->DestroyArea();
	}
	return true;
}

void UUtilityAIFunctionLibrary::StartAIDebugMode()
{
	FUtilityAIPluginModule::Get().StartAIDebugMode();
}

void UUtilityAIFunctionLibrary::EndAIDebugMode()
{
	FUtilityAIPluginModule::Get().EndAIDebugMode();
}


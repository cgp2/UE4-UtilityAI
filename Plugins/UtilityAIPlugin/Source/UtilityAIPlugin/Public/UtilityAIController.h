// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UtilityAIPlugin/Actions/ActionManagerComponent.h"
#include "UtilityAIPlugin/Intentions/IntentionManagerComponent.h"
#include "UtilityAIPlugin/CharacterCharacteristics//BehaviourGroupManagerComponent.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/GameFramework/GameState.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "UtilityAIController.generated.h"

UENUM(BlueprintType) enum class EMovementType : uint8
{
	MT_ToArea UMETA(DisplayName = "ToArea"),
	MT_ToPoI UMETA(DisplayName = "ToPoI"),
	MT_ToLocation UMETA(DisplayName = "ToLocation"),
};


//TODO
/*
Test each stage unpause
*/
UENUM(BlueprintType) enum class EAIStage : uint8
{
	AS_IntentionChoosing UMETA(DisplayName = "IntentionChoosing"),
	AS_ActionChoosing UMETA(DisplayName = "ActionChoosing"),
	AS_PoIChoosing UMETA(DisplayName = "PoIChoosing"),
	AS_Waiting UMETA(DisplayName = "Waiting"),
	AS_Interaction UMETA(DisplayName = "Interaction"),
	AS_Moving UMETA(DisplayName = "Moving"),
};

/**
 *Base controller for UtilityAI Engine.
 Please inherit all your custom controllers from this controller
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementFinished, EMovementType, MovementType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementFailed, EMovementType, MovementType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnBeginDestroy, APawn*, pawn);

UCLASS()
class UTILITYAIPLUGIN_API AUtilityAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUtilityAIController();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Intention")
		FGuid ID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Intention")
		EAIStage CurrentStage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		UBehaviourGroupManagerComponent* BehaviorGroupManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		UIntentionManagerComponent* IntentionManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		UActionManagerComponent* ActionManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		EMovementType MovementType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		bool IsTargetReached;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		bool IsInitialized;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
		AActor* CurrentMovementTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
		FVector CurrentMovementTargetLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
		AAreaOfInterest* CurrentArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Actions")
		FName RoleName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
		bool IsPaused;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		bool CanStrafe;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		bool IsStoppedOnOverlap;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		bool IsPathfindingEnabled;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		bool IsProjectedDestinationToNavigation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		bool IsPartialPathAllowed;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(bool& isSuccess);
	UFUNCTION(BlueprintCallable, Category = "Navigation")
		bool GetRandomPointInRadius(const FVector& Origin, float Radius, TSubclassOf<UNavigationQueryFilter> NavFilter, FVector& OutResult);
	UFUNCTION(BlueprintCallable, Category = "Navigation")
		bool CheckPointReachable(const FVector Point, const FVector Extent = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StartMovementToTarget(AActor* target);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StartMovementToLocation(FVector location);
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void EndMovement();

	UFUNCTION(BlueprintCallable, Category = "Target")
		void GetAllAreasOfInterest(TArray<AAreaOfInterest*>& AreasOfInterest);
	UFUNCTION(BlueprintCallable, Category = "Target")
		void FindAreaOfInterest(FName AreaName, AAreaOfInterest*& AreaOfInterest);
	UFUNCTION(BlueprintCallable, Category = "Target")
		void GetAllPointsOfInterest(TArray<UPointOfInterestComponent*>& PointsOfInterest);
	UFUNCTION(BlueprintCallable, Category = "Target")
		void FindPointOfInterest(FName PoIName, UPointOfInterestComponent*& PointOfInterest);

	UFUNCTION(BlueprintCallable, Category = "Base")
		void GetGameState(AGameStateBase*& GameState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
		void MoveToArea(AAreaOfInterest* area);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
		void MoveToPoint(UPointOfInterestComponent* poi);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
		void Reappoint();
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void MovementFailed(AActor* target);
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	//socket name, mesh name

	UFUNCTION(BlueprintCallable, Category = "Base")
		void AttachActorToParent(USkeletalMeshComponent* targetMesh, FName socketName, FTransform transform, UStaticMesh* mesh, AStaticMeshActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, Category = "Base")
		void PauseAI(float duration = -1.0f);
	UFUNCTION(BlueprintCallable, Category = "Base")
		void UnpauseAI();
	UFUNCTION(BlueprintCallable, Category = "Base")
		void ChangeAIStage(EAIStage NewStage);

#if WITH_EDITORONLY_DATA
	UPROPERTY()
		FTimerHandle DebugTickTimerHandler;
	UFUNCTION(BlueprintCallable, Category = "Debug")
		void StartDebugMode();
	UFUNCTION(BlueprintCallable, Category = "Debug")
		void EndDebugMode();
	UFUNCTION(Category = "Debug")
		void DebugModeTick();
		void OnObjectSelected(UObject* Object);
	UPROPERTY()
		bool IsDebugMode;
#endif

	//virtual void OnPossess(APawn* PossessedPawn);

	UPROPERTY(BlueprintAssignable)
		FOnMovementFinished OnMovementFinished;
	UPROPERTY(BlueprintAssignable)
		FOnMovementFailed OnMovementFailed;
	UPROPERTY(BlueprintAssignable)
		FOnPawnBeginDestroy OnPawnBeginDestroy;



protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		bool FloatEqualWithTollerance(float value1, float value2, float tollerance);
	UPROPERTY()
		int32 MovementAttempts;
	UPROPERTY()
		float MovementPrecision;

private:
	UPROPERTY()
		FTimerHandle UPauseTimerHandler;

	UPROPERTY()
		EAIStage pausedStage;

};

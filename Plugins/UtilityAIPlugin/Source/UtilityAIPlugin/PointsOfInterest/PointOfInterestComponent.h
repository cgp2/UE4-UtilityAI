// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "UtilityAIPlugin/PointsOfInterest/AreaOfInterest.h"
#include "UtilityAIPlugin//Actions/ActionAbstract.h"
#include "UtilityAIPlugin/Intentions/IntentionManagerComponent.h"
#include "UtilityAIPlugin/PointsOfInterest/UtilityObject.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "PointOfInterestComponent.generated.h"

UENUM(BlueprintType) enum class EPointUtilityCalculationFunction : uint8
{
	CF_Distance UMETA(DisplayName = "Distance"),
	CF_Random UMETA(DisplayName = "Random"),
	CF_RandomDistance UMETA(DisplayName = "Random+Distance"),
	CF_CapacityDistance UMETA(DisplayName = "Capacity+Distance"),
	CF_OccupancyRandom UMETA(DisplayName = "Occupancy+Random"),
	CF_Custom UMETA(DisplayName = "Custom"),

};

UENUM(BlueprintType) enum class EPoIUtilitySatisfyCondition : uint8
{
	SC_InitialSatisfied UMETA(DisplayName = "InitialSatisfied"),
	SC_InteractionTime UMETA(DisplayName = "InteractionTime"),
	SC_UtilityLevel UMETA(DisplayName = "UtilityLevel"),
	SC_Custom UMETA(DisplayName = "Custom"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartInteraction, class APawn*, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteraction, class APawn*, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostInitialize);

UCLASS(BlueprintType, Blueprintable, ClassGroup = UtilityAI, meta=(BlueprintSpawnableComponent))
class UTILITYAIPLUGIN_API UPointOfInterestComponent : public USceneComponent/*, public IUtilityObject*/
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPointOfInterestComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		FName Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		FGuid ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		int32 MaxUsers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		float AcceptanceRadius;
	UPROPERTY(BlueprintReadWrite, Category = "Base")
		AAreaOfInterest* ParentArea;
	UPROPERTY(BlueprintReadOnly, Category = "Base")
		USphereComponent* SphereCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		float MinInteractionTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		float SatisfyUtilityLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		EPoIUtilitySatisfyCondition IntentionSatisfyCondition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		TMap<FGuid, APawn*> Users;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		TMap<FGuid, APawn*> QueueUsers;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		TMap<FGuid, APawn*> AppointedUsers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Users")
		TArray<FName> UsersRoles;
	UPROPERTY(BlueprintReadOnly, Category = "Users")
		TMap<FGuid, float> UsersInteractionTime;
	UPROPERTY(BlueprintReadOnly, Category = "Users")
		TMap<FGuid, bool> UsersIntentionsSatisfied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
		bool IsActivated;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flags")
		bool IsOccupied;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsInitialized;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsAttachedToArea;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsDistanceWithZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intentions")
		EPointUtilityCalculationFunction CalculationFunctionType;
	//Indicates if intention is minimized after interaction finished
	UPROPERTY(BlueprintReadWrite, Category = "Intentions")
		bool IsIntentionMinimaized;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intentions")
		TMap<TSubclassOf<UIntentionAbstract>, float> SatisfiedIntentions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intentions")
		TMap<FGuid, TSubclassOf<UIntentionAbstract>> UsersIntentions;

	UFUNCTION(BlueprintCallable, Category = "Base")
		void Initialize(bool& isSuccess);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void ProceedInteraction(APawn* Pawn);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void FinishInteraction(APawn* Pawn);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void AppointVisitor(APawn* Pawn);

	UPROPERTY(BlueprintAssignable)
		FOnStartInteraction OnStartInteraction;
	//UPROPERTY(BlueprintAssignable)
	//	FOnPawnBeginDestroy OnPawnBeginDestroy;

	UPROPERTY(BlueprintAssignable)
		FOnStartInteraction OnEndInteraction;
	UPROPERTY(BlueprintAssignable)
		FOnPostInitialize OnPostInitialize;

	UFUNCTION(BlueprintCallable, Category = "Base")
		void CalculateUtility(APawn* Pawn, UIntentionAbstract* IntentionObj, float& Utility);
	//virtual void CalculateUtility_Implementation(APawn* Pawn, UIntentionAbstract* IntentionObj, float& Utility) override;

	UFUNCTION()
		void OnPawnBeginDestroy(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category = "Base")
		void DestroyPoI(bool isParentActorDestroyed = true);

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
//	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Events")
		void StartInteraction(APawn* Pawn);

	//Override this to implement custom utility function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void UtilityFunction(APawn* Pawn, float& Utility);

	UFUNCTION(BlueprintCallable, Category = "Intentions")
		bool CheckIntentionContainment(TSubclassOf<UIntentionAbstract> intention);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};

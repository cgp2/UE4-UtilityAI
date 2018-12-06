// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
//#include "UtilityAIPlugin/Public/UtilityAIController.h"
#include "UtilityAIPlugin/Intentions/IntentionAbstract.h"
#include "ActionAbstract.generated.h"

class AUtilityAIController;
/**
 * 
 */
UENUM(BlueprintType) enum class EActionTargetType : uint8
{
	AT_Inner UMETA(DisplayName = "Inner"),
	AT_Outer UMETA(DisplayName = "Outer"),
};

UENUM(BlueprintType) enum class EIntentionSatisfyCondition : uint8
{
	SC_InitialSatisfied UMETA(DisplayName = "InitialSatisfied"),
	SC_UtilityLevel UMETA(DisplayName = "UtilityLevel"),
	SC_Custom UMETA(DisplayName = "Custom"),
};

UENUM(BlueprintType) enum class EActionState : uint8
{
	AS_InProgress UMETA(DisplayName = "InProgress"),
	AS_Paused UMETA(DisplayName = "Paused"),
	AS_Finished UMETA(DisplayName = "Finished"),
};

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = UtilityAI)
class UTILITYAIPLUGIN_API UActionAbstract : public UObject 
{
	GENERATED_BODY()
	
public:
	UActionAbstract();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base")
		FGuid ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		TArray<TSubclassOf<UIntentionAbstract>> IntentionsClasses;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base")
		AUtilityAIController* OwnerController;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base", meta = (ExposeOnSpawn = "true"))
		APawn* OwnerPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsTickable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsRoleSpecified;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsIndividual;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsStoppedWithUtilityChanged;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsTimeLimited;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags")
		bool IsMovementStoppedOnStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flags", meta = (EditCondition = "IsTickable"))
		bool IsChangingUrge;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Intention", meta = (EditCondition = "IsChangingUrge"))
		float NewUrgeGrowthRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Intention", meta = (EditCondition = "IsChangingUrge"))
		TSubclassOf<UIntentionAbstract> AffectedIntentionClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Intention")
		UIntentionAbstract* AffectedIntention;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Intention")
		EIntentionSatisfyCondition SatisfyConditionType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Intention")
		float SatisfyUtilityLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Action")
		EActionState ActionState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
		EActionTargetType ActionType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
		FName ActionSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
		int32 Priority;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
		int32 Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
		float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
		float CurrentDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
		TArray<FName> ActorsRolesNames;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(APawn* owner, bool& isInitialized);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void CalculateUtility(UIntentionAbstract* CurrentIntention, float& Utility);

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
	UFUNCTION(BlueprintCallable, Category = "Base")
		bool GetRandomPointInRadius(const FVector& Origin, float Radius, TSubclassOf<UNavigationQueryFilter> NavFilter, FVector& OutResult);

	UFUNCTION()
		void ReceiveTick(float deltaTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void Tick(float deltaTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void ExecutionCheck(APawn* owner, EActionState& NewState);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void StartExecution();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnStart();
	UFUNCTION(BlueprintCallable, Category = "Events")
		void StopExecution();
	UFUNCTION(BlueprintCallable, Category = "Events")
		void BreakAction();
	UFUNCTION(BlueprintCallable, Category = "Events")
		void PauseExecution();
	UFUNCTION(BlueprintCallable, Category = "Events")
		void UnpauseExecution();
	UFUNCTION(BlueprintCallable, Category = "Events")
		void FindTarget(float& maxUtility, UIntentionAbstract* intention, FGuid& maxID);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnStop();


	FORCEINLINE bool operator < (const UActionAbstract &Other) const
	{
		return Priority < Other.Priority;
	}
	
protected:
	UPROPERTY()
		FGuid TargetID;
	UFUNCTION()
		void ValidateTarget(FGuid targetMaxID);


	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
//#include "UtilityAIPlugin/Public/UtilityAIController.h"
#include "IntentionAbstract.generated.h"

class AUtilityAIController;

UENUM(BlueprintType) enum class EUtilityCalculationFuction : uint8
{
	UT_Custom UMETA(DisplayName = "Custom"),
	UT_Sigmoid UMETA(DisplayName = "Sigmoid"),
	UT_Tanh UMETA(DisplayName = "Tanh"),
	UT_MaxAction UMETA(DisplayName = "MaxActionUtility"),
	UT_ConstMax UMETA(DisplayName = "ConstantMax")
	/*UT_NeuralNetwork UMETA(DisplayName = "NeuralNetwork"),*/
};
UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = UtilityAI)
class UTILITYAIPLUGIN_API UIntentionAbstract : public UObject
{
	GENERATED_BODY()

public:
	UIntentionAbstract();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		FName Group;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base")
		FGuid ID;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base", meta = (ExposeOnSpawn = "true"))
		APawn* OwnerPawn;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base")
		AUtilityAIController* OwnerController;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base", meta = (ExposeOnSpawn = "true"))
		AActor* Instigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		EUtilityCalculationFuction UtilityFunctionType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Utility")
		float Utility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		float Urge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		float initialGrowthRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		float GrowthRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		float MinUtility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		float MaxUtility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		bool IsInteruptable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		bool IsRandomizable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		bool IsObligatory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		bool IsSatisfied;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility")
		bool IsTargeted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility", meta = (EditCondition = "IsTargeted"))
		FName TargetName;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(APawn* owner, bool& isInitialized);

	/*Calculates utility according to urge and growth rate value*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void CalculateUtility(float& NewUtility);

	UFUNCTION(BlueprintImplementableEvent, Category = "Utility")
		void UtilityFunction(float& NewUtility);

	/*Randomize urge and return new utility value*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void Randomize(float& NewUtility);

	/*Set utility to MinUtility value urge and return new utility value*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void MinimizeUtility(const bool MinimaizeGrowth, float& NewUtility);

	/*Set utility to MaxUtility value urge and return new utility value*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void MaximizeUtility(float& NewUtility);

	UFUNCTION(BlueprintCallable, Category = "Utility")
		void ChangeGrowthRate(float NewGrowthRate);

	/*Returns GrowthRate to initial value*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void RestoreInitialGrowthRate(float& NewGrowthRate);

	/*Returns GrowthRate to last recorded value*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void RestoreLastGrowthRate(float &NewGrowthRate);

	UFUNCTION(BlueprintCallable, Category = "Events")
		void ExecuteTick(float deltaTime);

	/*Called when Character accepts this utility as dominant*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnApply();

	/*Called when Character changes this utility*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnDetach();

	/*Called when this utility is added for Character*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnSpawn();


	FORCEINLINE bool operator==(const UIntentionAbstract &Other) const
	{
		return ID == Other.ID;
	}

protected:
	UFUNCTION(BlueprintCallable, Category = "Utility")
		void CalculateUrge(float deltaTime, float& NewUrge);

	UFUNCTION(BlueprintCallable, Category = "Utility")
		void CheckUtilityRange(const float UtValue, float& NewUtility);

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
		void ReceiveTick(float deltaTime);

	UPROPERTY()
		float lastGrowthRate;
};

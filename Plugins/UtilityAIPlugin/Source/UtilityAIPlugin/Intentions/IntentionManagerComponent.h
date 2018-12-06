// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UtilityAIPlugin/Intentions/IntentionAbstract.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "IntentionManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntentionChanged, class UIntentionAbstract*, NewIntention);

UCLASS(Blueprintable, ClassGroup = UtilityAI, meta = (BlueprintSpawnableComponent))
class UTILITYAIPLUGIN_API UIntentionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UIntentionManagerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intention")
		TArray<TSubclassOf<UIntentionAbstract>> IntentionClasses;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Intention")
		int32 IntentionCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intention")
		float IntentionsUpdateTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intention")
		float DeceisionMakingTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Intention")
		TMap<FGuid, UIntentionAbstract*> Intentions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flags")
		bool IsBusy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
		bool IsStopped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
		bool IsPaused;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(bool& isInitialized);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void PostInitialize(bool& isInitialized);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void AddIntentionByClass(TSubclassOf<UIntentionAbstract> IntentionClass, UIntentionAbstract*& AddedIntention);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void AddIntention(UIntentionAbstract* Intention);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void RemoveIntentionByClass(const TSubclassOf<UIntentionAbstract> IntentionClass, bool& isSuccess);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void MinimizeIntention(const TSubclassOf<UIntentionAbstract> IntentionClass, const bool MinimaizeGrowth, float& NewUtility);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void MaximizeIntention(const TSubclassOf<UIntentionAbstract> IntentionClass, float& NewUtility);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void RandomizeIntention(const TSubclassOf<UIntentionAbstract> IntentionClass, float& NewUtility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Intention")
		void PauseUpdate(float duration = -1.0f);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Intention")
		void UnpauseUpdate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Intention")
		void ValidateChoice(UIntentionAbstract* MaxIntentionObj);

	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Intention")
		void ValidateChoiñe(UIntentionAbstract* MaxIntentionObj);*/

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		UIntentionAbstract* GetCurrentIntention();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		UIntentionAbstract* GetPreviousIntention();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		void GetIntentionByID(FGuid IntentionID, UIntentionAbstract *&Intention);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		void GetIntentionByClass(const TSubclassOf<UIntentionAbstract> IntentionClass, UIntentionAbstract *&Intention);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		void GetIntentionsByGroupName(FName GroupName, TArray<UIntentionAbstract*>& IntentionArray);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void MinimizeCurrentIntention(const bool MinimaizeGrowth, float& NewUtility);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void MaximizeCurrentUtility(float& NewUtility);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void RestoreIntentionGrowthRate(const TSubclassOf<UIntentionAbstract> IntentionClass, bool RestoreInitial, bool &IsSuccess);
	UFUNCTION(BlueprintCallable, Category = "Intention")
		void ChangeIntentionGrowthRate(const TSubclassOf<UIntentionAbstract> IntentionClass, float NewGrowthRate, bool &IsSuccess);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void ChangeCurrentIntentionGrowthRate(const float NewGrowthRate);

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void MakeDecision();

	UPROPERTY(BlueprintAssignable)
		FIntentionChanged OnIntentionChanged;

	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		float TickTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Intention")
		FGuid CurrentIntentionID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Intention")
		FGuid PreviousIntentionID;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Base")
		void GetOwnerPawn(AActor*& OwnerPawn);

	UFUNCTION()
		APawn* GetPawn();

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void ChooseIntention();

	UFUNCTION(BlueprintCallable, Category = "Intention")
		void UpdateIntentions();

	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		void GetCurrentIntentionID(FGuid& CurIntentionID);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		void GetPreviousIntentionID(FGuid& PrevIntentionID);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Intention")
		FGuid GetIntentionIDByClass(const TSubclassOf<UIntentionAbstract> IntentionClass);

private:
	UPROPERTY()
		float pauseTime;
	UPROPERTY()
		float pauseDuration;
	UPROPERTY()
		bool isValidChoise;
	UPROPERTY()
		FTimerHandle UpdateIntentionsTimerHandler;
	UPROPERTY()
		FTimerHandle ChooseIntentionTimerHandler;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};

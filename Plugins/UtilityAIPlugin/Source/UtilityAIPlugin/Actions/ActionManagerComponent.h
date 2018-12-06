// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ActionAbstract.h"
#include "ActionManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionChanged, TArray<UActionAbstract*>, CurrentActions);

UCLASS(Blueprintable, ClassGroup = UtilityAI, meta = (BlueprintSpawnableComponent))
class UTILITYAIPLUGIN_API UActionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActionManagerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
		TArray<TSubclassOf<UActionAbstract>> ActionClasses;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Actions")
		TMap<FGuid, UActionAbstract*> PossibleActions;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Actions")
		TArray<UActionAbstract*> CurrentActions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions", Meta = (UIMin = 0.0f))
		int32 MaxSimultaneousActionCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions", Meta = (UIMin = 0.0f))
		float ActionChoosingTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Actions", Meta = (UIMin = 0.0f))
		int32 CurrentActionCost;
	UPROPERTY(BlueprintAssignable)
		FActionChanged OnActionsChanged;
	UPROPERTY(VisibleAnywhere, Category = "Base")
		bool IsPaused;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(bool& isInitialized);

	//UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Actions")
	//	void GetCurrentActions(TArray<UActionAbstract*> Actions);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddActionByClass(TSubclassOf<UActionAbstract> ActionClass, UActionAbstract*& AddedAction);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddAction(UActionAbstract* NewAction);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void ChooseActions(UIntentionAbstract *CurrentIntention);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void OnIntentionChanged(UIntentionAbstract *CurrentUtility);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void BreakActionByName(FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void BreakActionByClass(TSubclassOf<UActionAbstract> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void PauseActions(float duration = -1.0f);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void UnpauseActions();

	UFUNCTION(BlueprintCallable, Category = "Actions")
		float FindMaxActionUtility(UIntentionAbstract *CurrentIntention, UActionAbstract*& MaxAction);



protected:

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void ExecutionTick(float deltaTime);

	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
		FTimerHandle ChooseActionsTimerHandler;
	UPROPERTY()
		FTimerHandle ActionExecutionTimerHandler;
	UFUNCTION()
		bool CheckActionSlotAvaliability(const UActionAbstract* action);
	UPROPERTY()
		float pauseDuration;
	UPROPERTY()
		float pauseTime;
};

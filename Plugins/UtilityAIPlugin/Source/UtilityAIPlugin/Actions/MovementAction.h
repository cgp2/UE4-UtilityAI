// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbstract.h"
#include "MovementAction.generated.h"

/**
 * 
 */
UCLASS()
class UTILITYAIPLUGIN_API UMovementAction : public UActionAbstract
{
	GENERATED_BODY()
	
public:
	UMovementAction();
	
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
	//	 void ExecutionCheck(APawn* owner, EActionState& NewState) override;
	
	//virtual void ExecutionCheck_Implementation(APawn* owner, EActionState& NewState) override;
	virtual void OnStop_Implementation() override;
	virtual void CalculateUtility_Implementation(UIntentionAbstract* CurrentIntention, float& Utility) override;
};

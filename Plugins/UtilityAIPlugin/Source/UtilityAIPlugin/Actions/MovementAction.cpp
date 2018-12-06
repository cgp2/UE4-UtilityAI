// Fill out your copyright notice in the Description page of Project Settings.

#include "MovementAction.h"

UMovementAction::UMovementAction()
{
	IsTickable = false;
	ActionState = EActionState::AS_Finished;
	IsIndividual = false;
	IsTimeLimited = false;
	IsStoppedWithUtilityChanged = true;
	ActionSlot = FName("Base");
	ActionType = EActionTargetType::AT_Outer;
	Priority = 1;
	Cost = 1;
	Duration = 0.0f;
	CurrentDuration = 0.0f;
	SatisfyConditionType = EIntentionSatisfyCondition::SC_Custom;
}


void UMovementAction::OnStop_Implementation()
{
	UPointOfInterestComponent* poi = OwnerController->CurrentMovementTarget->FindComponentByClass<UPointOfInterestComponent>();
	if (poi->IsValidLowLevel())
	{
		if (OwnerController->IsTargetReached)
			poi->FinishInteraction(OwnerPawn);
		else
			poi->AppointedUsers.Remove(OwnerController->ID);
	}
}

void UMovementAction::CalculateUtility_Implementation(UIntentionAbstract* CurrentIntention, float& Utility)
{
	FGuid maxID;
	FindTarget(Utility, CurrentIntention, maxID);
}

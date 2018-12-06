// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionManagerComponent.h"
#include "UtilityAIPlugin/Intentions/IntentionManagerComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UtilityAIPlugin/Public/UtilityAIController.h"


// Sets default values for this component's properties
UActionManagerComponent::UActionManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaxSimultaneousActionCost = 2;
	ActionChoosingTime = 0.2f;
	CurrentActionCost = 0.0f;
	IsPaused = false;
	pauseDuration = 0.0f;
	pauseTime = 0.0f;
	// ...
}

// Called when the game starts
void UActionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UActionManagerComponent::Initialize_Implementation(bool& isInitialized)
{
	UIntentionManagerComponent* utComp = Cast<UIntentionManagerComponent>(GetOwner()->FindComponentByClass<class UIntentionManagerComponent>());
	if (utComp)
		utComp->OnIntentionChanged.AddDynamic(this, &UActionManagerComponent::OnIntentionChanged);

	for (TSubclassOf<UActionAbstract> actionClass : ActionClasses)
	{
		if(actionClass->IsValidLowLevelFast())
		{
			bool isActionInitialized = false;

			UActionAbstract *tempAction = NewObject<UActionAbstract>(this, actionClass);
			tempAction->Initialize(Cast<APawn>(Cast<AUtilityAIController>(GetOwner())->GetPawn()), isActionInitialized);
			if (isActionInitialized)
				PossibleActions.Add(tempAction->ID, tempAction);
		}
	}

	isInitialized = true;
}

void UActionManagerComponent::AddActionByClass(TSubclassOf<UActionAbstract> ActionClass, UActionAbstract*& AddedAction)
{
	if(ActionClass->IsValidLowLevelFast())
	{
		bool isInitialized = false;
		if (!ActionClasses.Contains(ActionClass))
		{
			UActionAbstract *tempAction = NewObject<UActionAbstract>(this, ActionClass);

			tempAction->Initialize(Cast<APawn>(Cast<AUtilityAIController>(GetOwner())->GetPawn()), isInitialized);
			if (isInitialized)
			{
				ActionClasses.Add(ActionClass);
				PossibleActions.Add(tempAction->ID, tempAction);
				AddedAction = tempAction;
			}
		}
	}
}

void UActionManagerComponent::AddAction(UActionAbstract* NewAction)
{
	if(NewAction->IsValidLowLevelFast())
	{
		bool isInitialized = false;
		if (!ActionClasses.Contains(NewAction->GetClass()))
		{
			NewAction->Initialize(Cast<APawn>(Cast<AUtilityAIController>(GetOwner())->GetPawn()), isInitialized);
			PossibleActions.Add(NewAction->ID, NewAction);
			ActionClasses.Add(NewAction->GetClass());
		}
	}
}

void UActionManagerComponent::OnIntentionChanged(UIntentionAbstract* CurrentUtility)
{
	(Cast<AUtilityAIController>(GetOwner()))->ChangeAIStage(EAIStage::AS_ActionChoosing);
	//Pause and finish current actions
	if (CurrentActions.Num() > 0)
	{
		int i = 0;
		while (i < CurrentActions.Num())
		{
			//finish action with such flag
			if (CurrentActions[i]->IsStoppedWithUtilityChanged)
			{
				BreakActionByName(CurrentActions[i]->Name);
				i--;
			}
			else
				if (CurrentActions[i]->ActionState == EActionState::AS_Finished)
				{
					BreakActionByName(CurrentActions[i]->Name);
					i--;
				}
				else
					CurrentActions[i]->PauseExecution();
			i++;
		}
	}

	ChooseActions(CurrentUtility);
}

void UActionManagerComponent::ChooseActions(UIntentionAbstract *CurrentIntention)
{
	UActionAbstract* choosenAction = nullptr;
	FindMaxActionUtility(CurrentIntention, choosenAction);

	if (choosenAction)
	{
		if (!(choosenAction->IsIndividual))
		{
			CurrentActionCost = choosenAction->Cost;
			int i = 0;
			while (i < CurrentActions.Num())
			{
				if (CurrentActions[i] == choosenAction)
				{
					CurrentActions.RemoveAt(i);
					i--;
				}
				else
				{
					if (CurrentActions[i]->Cost + CurrentActionCost <= MaxSimultaneousActionCost)
					{
						if (CurrentActions[i]->ActionSlot == choosenAction->ActionSlot)
						{
							CurrentActions.RemoveAt(i);
							i--;
						}
						else
						{
							CurrentActionCost += CurrentActions[i]->Cost;
							CurrentActions[i]->UnpauseExecution();
						}
					}
					else
					{
						CurrentActions.RemoveAt(i);
						i--;
					}
				}
				i++;
			}		
		}
		choosenAction->StartExecution();
		CurrentActions.Add(choosenAction);
		CurrentActions.Sort();
		OnActionsChanged.Broadcast(CurrentActions);
	}
	else
	{
		CurrentIntention->Urge -= 0.3f;
	}
}

void UActionManagerComponent::BreakActionByName(FName ActionName)
{
	if(ActionName.IsValid())
	{
		for (int i = 0; i < CurrentActions.Num(); i++)
		{
			if (CurrentActions[i]->Name == ActionName)
			{
				CurrentActions[i]->ActionState = EActionState::AS_Finished;
				CurrentActions[i]->StopExecution();
				CurrentActionCost -= CurrentActions[i]->Cost;
				CurrentActions.RemoveAt(i);
				return;
			}
		}
	}
}
//TODO action/intention satisfaction  (flag satisfy by default in action)
// action cant be changed till current intention is satisfied
void UActionManagerComponent::BreakActionByClass(TSubclassOf<UActionAbstract> ActionClass)
{
	if(ActionClass->IsValidLowLevelFast())
	{
		for (int i = 0; i < CurrentActions.Num(); i++)
		{
			if (CurrentActions[i]->GetClass() == ActionClass)
			{
				CurrentActions[i]->ActionState = EActionState::AS_Finished;
				CurrentActions[i]->StopExecution();
				CurrentActionCost -= CurrentActions[i]->Cost;
				CurrentActions.RemoveAt(i);
				return;
			}
		}
	}
}

void UActionManagerComponent::PauseActions(float duration /*= -1.0f*/)
{
	IsPaused = true;
	for (UActionAbstract* action : CurrentActions)
	{
		action->PauseExecution();
	}
	if (duration != -1)
	{
		pauseDuration = duration;
		pauseTime = 0.0f;
	}
}

void UActionManagerComponent::UnpauseActions()
{
	IsPaused = false;
	for (UActionAbstract* action : CurrentActions)
	{
		action->UnpauseExecution();
	}
	pauseDuration = 0.0f;
	pauseTime = 0.0f;
}

float UActionManagerComponent::FindMaxActionUtility(UIntentionAbstract *CurrentIntention, UActionAbstract*& MaxAction)
{
	TArray<UActionAbstract*> actions;
	PossibleActions.GenerateValueArray(actions);
	float choosenUtility = -1000.0f;
	for (UActionAbstract* action : actions)
	{
		float actionUtility;
		action->CalculateUtility(CurrentIntention, actionUtility);
		if (MaxAction == nullptr)
		{
			MaxAction = action;
			choosenUtility = actionUtility;
		}
		else
		{
			if (actionUtility > choosenUtility)
			{
				if ((action->Cost + CurrentActionCost) < MaxSimultaneousActionCost)
				{
					MaxAction = action;
					choosenUtility = actionUtility;
				}
			}
		}
	}
	return choosenUtility;
}

void UActionManagerComponent::ExecutionTick(float deltaTime)
{
	int i = 0;
	if (IsPaused)
	{
		pauseTime += deltaTime;
		if (pauseTime >= pauseDuration)
		{
			UnpauseActions();
		}
	}

	while (i < CurrentActions.Num())
	{
		if (CurrentActions[i]->ActionState != EActionState::AS_Paused)
		{
			EActionState actionState;
			CurrentActions[i]->ExecutionCheck(Cast<AAIController>(GetOwner())->GetPawn(), actionState);
			if (actionState != EActionState::AS_Finished)
			{
				if (CurrentActions[i]->IsTickable)
				{
					CurrentActions[i]->ReceiveTick(deltaTime);
					//Check if action duration expired
					
				}
			}
			else
			{
				BreakActionByName(CurrentActions[i]->Name);
				i--;
			}
		}
		i++;
	}
}

// Called every frame
void UActionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ExecutionTick(DeltaTime);

	// ...
}

bool UActionManagerComponent::CheckActionSlotAvaliability(const UActionAbstract* action)
{
	if(action->IsValidLowLevel())
	{
		bool isSlotAvailable = true;
		for (UActionAbstract* curAction : CurrentActions)
		{
			if (curAction->GetClass() != action->GetClass())
			{
				if (curAction->ActionSlot == action->ActionSlot)
				{
					isSlotAvailable = false;
					break;
				}
			}
		}
		return isSlotAvailable;
	}

	return false;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "IntentionAbstract.h"
#include "UtilityAIPlugin/Actions/ActionManagerComponent.h"

UIntentionAbstract::UIntentionAbstract()
{
	IsRandomizable = false;
	IsInteruptable = true;
	IsObligatory = false;
	Utility = 0.0f;
	Urge = 0.0f;
	MaxUtility = 0.7f;
	MinUtility = 0.0f;
	GrowthRate = 0.2f;
	Group = FName("Base");
	IsSatisfied = false;
	IsTargeted = false;
	TargetName = "None";
	UtilityFunctionType = EUtilityCalculationFuction::UT_Tanh;
}

void UIntentionAbstract::Initialize_Implementation(APawn* owner, bool& isInitialized)
{
	ID = FGuid().NewGuid();
	initialGrowthRate = GrowthRate;
	OwnerPawn = owner;
	OwnerController = Cast<AUtilityAIController>(OwnerPawn->GetController());

	if (IsRandomizable)
		Randomize(Utility);
	isInitialized = true;

	OnSpawn();
}

void UIntentionAbstract::CalculateUrge(float deltaTime, float& NewUrge)
{
	float ratio = GrowthRate * deltaTime;
	Urge = FMath::Max(0.0f, Urge + ratio);
	if (IsObligatory)
	{
		Urge = FMath::Min(0.95f, Urge);
	}
	else
	{
		Urge = FMath::Min(0.8f, Urge);
	}

	NewUrge = Urge;
}

void UIntentionAbstract::CheckUtilityRange(const float UtValue, float& NewUtility)
{
	float tempUt = FMath::Max(MinUtility, UtValue);
	tempUt = FMath::Min(MaxUtility, tempUt);

	NewUtility = tempUt;
}

void UIntentionAbstract::CalculateUtility(float& NewUtility)
{
	switch (UtilityFunctionType)
	{
	case EUtilityCalculationFuction::UT_Custom:
		UtilityFunction(NewUtility);
		break;	
	case EUtilityCalculationFuction::UT_Sigmoid:
		NewUtility = 1.0f / (1 + FMath::Exp(-1.0f * Urge));
		break;
	case EUtilityCalculationFuction::UT_Tanh:
		NewUtility = (FMath::Exp(Urge) - FMath::Exp(-1.0f * Urge)) / (FMath::Exp(Urge) + FMath::Exp(-1.0f * Urge));
		break;
	case EUtilityCalculationFuction::UT_MaxAction:
	{
		UActionAbstract* maxAction;
		NewUtility = OwnerController->ActionManager->FindMaxActionUtility(this, maxAction);
		break;
	}
	case EUtilityCalculationFuction::UT_ConstMax:
		Utility = MaxUtility;
		break;
	default:
		NewUtility = Urge;
		break;
	}
	CheckUtilityRange(Utility, NewUtility);
}

void UIntentionAbstract::Randomize(float& NewUtility)
{
	Urge = FMath::FRandRange(0.0f, 1.0f);
	
	//CalculateUtility(NewUtility);
}

void UIntentionAbstract::MinimizeUtility(const bool MinimaizeGrowth, float& NewUtility)
{
	Urge = 0.0f;
	if (MinimaizeGrowth)
	{
		lastGrowthRate = GrowthRate;
		GrowthRate = 0.0f;
	}

	CalculateUtility(NewUtility);
}

void UIntentionAbstract::MaximizeUtility(float& NewUtility)
{
	Urge = 1.0f;
	CalculateUtility(NewUtility);
}

void UIntentionAbstract::ChangeGrowthRate(float NewGrowthRate)
{
	lastGrowthRate = GrowthRate;
	GrowthRate = NewGrowthRate;
}

void UIntentionAbstract::RestoreInitialGrowthRate(float& NewGrowthRate)
{
	lastGrowthRate = GrowthRate;
	GrowthRate = initialGrowthRate;
	NewGrowthRate = GrowthRate;
}

void UIntentionAbstract::RestoreLastGrowthRate(float& NewGrowthRate)
{
	GrowthRate = lastGrowthRate;
	NewGrowthRate = GrowthRate;
}

void UIntentionAbstract::ExecuteTick(float deltaTime)
{
	CalculateUrge(deltaTime, Urge);
	CalculateUtility(Utility);
	ReceiveTick(deltaTime);
}

void UIntentionAbstract::ReceiveTick_Implementation(float deltaTime)
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), Utility));
}

void UIntentionAbstract::OnApply_Implementation()
{
	
}

void UIntentionAbstract::OnDetach_Implementation()
{

}

void UIntentionAbstract::OnSpawn_Implementation()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hi i am spawned")));*/
}



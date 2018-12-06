// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UtilityAIPlugin/Public/AIActorsManager.h"
#include "BehaviorGroupAbstract.h"
#include "UtilityAIPlugin/Actions/ActionManagerComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UtilityAIPlugin/CharacterCharacteristics/CharacteristicBase.h"
#include "BehaviourGroupManagerComponent.generated.h"

class UActionManagerComponent;
class AUtilityAIController;

UENUM(BlueprintType) enum class EBehaviorGroupType : uint8
{
	BG_Default UMETA(DisplayName = "Default"),
	BG_Characteristics UMETA(DisplayName = "Characteristics"),
};

USTRUCT(BlueprintType)
struct FCharacteristicParametrs
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (UIMin = 0, UIMax = 10))
		int32 Value;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (UIMin = 0.0f, UIMax = 1.0f))
		float Mult;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UTILITYAIPLUGIN_API UBehaviourGroupManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBehaviourGroupManagerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		TSubclassOf<UBehaviorGroupAbstract> BehaviourGroupClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base")
		EBehaviorGroupType BehaviorGroupType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Characteristics", meta = (EditCondition = "IsCharacteristics"))
		TMap<TSubclassOf<UCharacteristicBase>, FCharacteristicParametrs> CharacteristicsClasses;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Characteristics", meta = (EditCondition = "IsCharacteristics"))
		TArray<UCharacteristicBase*> Characteristics;

	UPROPERTY()
		TMap<TSubclassOf<UCharacteristicBase>, bool> DirtyMap;

	UPROPERTY()
		bool IsCharacteristics;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(bool& isInitialized);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void PostInitialize(bool& IsSuccess);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Base")
		UBehaviorGroupAbstract* BehaviourGroupObj;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void PostEditChangeProperty(struct FPropertyChangedEvent& e);

};

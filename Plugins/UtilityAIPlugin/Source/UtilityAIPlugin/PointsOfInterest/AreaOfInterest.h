// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UtilityAIPlugin/PointsOfInterest/UtilityObject.h"
#include "UtilityAIPlugin/Intentions/IntentionAbstract.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "AreaOfInterest.generated.h"

class UPointOfInterestComponent;

UENUM(BlueprintType) enum class EAreaCalculationFunction: uint8
{
	CF_MaxInnerPointsUtility UMETA(DisplayName = "MaxInnerPointsUtility"),
	CF_AverageInnerPointsUtility UMETA(DisplayName = "AverageInnerPointsUtility"),
	CF_Custom UMETA(DisplayName = "Custom"),
};

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = UtilityAI)
class UTILITYAIPLUGIN_API AAreaOfInterest : public AActor/*, public IUtilityObject*/
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAreaOfInterest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		FName Name;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base")
		FGuid ID;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base")
		bool IsInitialized;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Base")
		bool IsActivated;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base")
		UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base", meta = (MakeEditWidget = true))
		FVector BoxExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Users")
		bool IsLocalRolesAssigned;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		TMap<FGuid, APawn*> Users;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Users", meta = (EditCondition = "IsLocalRolesAssigned"))
		TMap<TSubclassOf<UIntentionAbstract>, FName> Roles;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		TMap<FGuid, FName> UsersRoles;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		TMap<FGuid, APawn*> AppointedUsers;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Users")
		int32 CurrentUserNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PointsOfInterest")
		EAreaCalculationFunction UtilityCalculationType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PointsOfInterest")
		TMap<FGuid, UPointOfInterestComponent*> PointsOfInterest;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void Initialize(bool& isSuccess);
	UFUNCTION(BlueprintCallable, Category = "Base")
		void CalculateUtility(APawn* Pawn, UIntentionAbstract* IntentionObj, float& Utility);
	/*virtual void CalculateUtility_Implementation(APawn* Pawn, UIntentionAbstract* IntentionObj, float& Utility) override;*/
	UFUNCTION(BlueprintCallable, Category = "PointsOfInterest")
		void AppointUserToPointOfInterest(FGuid UserID, UIntentionAbstract* Intention);
	UFUNCTION(BlueprintCallable, Category = "PointsOfInterest")
		void FindPointOfInterest(FGuid PointID, UPointOfInterestComponent*& poi);
	UFUNCTION(BlueprintCallable, Category = "PointsOfInterest")
		void FindPointOfInterestByName(FName PointName, UPointOfInterestComponent*& poi);
	//Override this to implement custom utility function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Base")
		void UtilityFunction(APawn* Pawn, float& Utility);

	UFUNCTION(BlueprintCallable, Category = "Users")
		void AppointUser(APawn* User);
	UFUNCTION(BlueprintCallable, Category = "Users")
		void ProcessOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION(BlueprintCallable, Category = "Users")
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION(Category = "Base")
		void OnPawnBeginDestroy(APawn* Pawn);
	UFUNCTION(BlueprintCallable, Category = "Base")
		void DestroyArea();

	/*virtual void BeginDestroy() override;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

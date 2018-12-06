// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "AreaGridCell.generated.h"

class AGridAreaOfInterest;

UCLASS()
class UTILITYAIPLUGIN_API AAreaGridCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAreaGridCell();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base")
		UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base")
		AGridAreaOfInterest* ParentArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base")
		TArray<APawn*> UsersInside;

	UFUNCTION(BlueprintCallable, Category = "Base")
		void Initialize(FVector Extent, AGridAreaOfInterest* Area);
	UFUNCTION(BlueprintCallable, Category = "Users")
		void ProcessOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION(BlueprintCallable, Category = "Users")
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
private:

};

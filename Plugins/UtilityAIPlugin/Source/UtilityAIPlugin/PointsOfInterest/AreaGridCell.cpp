// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaGridCell.h"
#include "GridAreaOfInterest.h"


// Sets default values

AAreaGridCell::AAreaGridCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	BoxComponent->ShapeColor = FColor::Green;
	RootComponent = BoxComponent;

	OnActorBeginOverlap.AddDynamic(this, &AAreaGridCell::ProcessOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAreaGridCell::EndOverlap);
}

void AAreaGridCell::Initialize(FVector Extent, AGridAreaOfInterest* Area)
{
	BoxComponent->SetBoxExtent(Extent);
	ParentArea = Area;
}

void AAreaGridCell::ProcessOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsValidLowLevel())
		return;

	if (!UsersInside.Contains(OtherActor))
	{
		APawn* pawn = Cast<APawn>(OtherActor);
		if(pawn->IsValidLowLevel())
			UsersInside.Add(pawn);
	}
	ParentArea->ProcessSellBeginOverlap(this, OtherActor);
}

void AAreaGridCell::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsValidLowLevel())
		return;

	if (UsersInside.Contains(OtherActor))
	{
		APawn* pawn = Cast<APawn>(OtherActor);
		if (pawn->IsValidLowLevel())
			UsersInside.Remove(pawn);
	}

	ParentArea->ProcessCellEndOverlap(this, OtherActor);
}
// Called when the game starts or when spawned
void AAreaGridCell::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAreaGridCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



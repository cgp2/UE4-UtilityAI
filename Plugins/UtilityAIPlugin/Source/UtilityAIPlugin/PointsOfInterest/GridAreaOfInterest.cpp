// Fill out your copyright notice in the Description page of Project Settings.

#include "GridAreaOfInterest.h"
#include "Runtime/Engine/Classes/AI/NavigationSystemBase.h"
#include "UtilityAIPlugin/Public/UtilityAIController.h"

AGridAreaOfInterest::AGridAreaOfInterest()
{
	CellHeight = 20.0f;
	CellWidth = 20.0f;
}

void AGridAreaOfInterest::AppointPawnToGridCell(APawn* pawn, AAreaGridCell* Cell)
{
	Cast<AUtilityAIController>(pawn->GetController())->StartMovementToLocation(Cell->GetActorLocation());
}

void AGridAreaOfInterest::GetCurrentGridCell(APawn* pawn, AAreaGridCell*& Cell)
{
	if (!pawn->IsValidLowLevel())
		return;

	if (UserCell.Contains(pawn))
		Cell = *UserCell.Find(pawn);
}

void AGridAreaOfInterest::ProcessSellBeginOverlap(AAreaGridCell* Cell, AActor* Actor)
{
	if (!Actor->IsValidLowLevel())
		return;

	APawn* pawn = Cast<APawn>(Actor);
	if (!pawn->IsValidLowLevel())
		return;

	if (UserCell.Contains(pawn))
		UserCell.Emplace(pawn, Cell);
	else
		UserCell.Add(pawn, Cell);
	OnCellBeginOverlap(Cell, Actor);
}

void AGridAreaOfInterest::ProcessCellEndOverlap(AAreaGridCell* Cell, AActor* Actor)
{
	if (!Actor->IsValidLowLevel())
		return;

	APawn* pawn = Cast<APawn>(Actor);
	if (!pawn->IsValidLowLevel())
		return;

	if (UserCell.Contains(pawn))
	{
		if (*UserCell.Find(pawn) == Cell)
			UserCell.Remove(pawn);
	}

	OnCellEndOverlap(Cell, Actor);
}


void AGridAreaOfInterest::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	BuildGrid();
}

void AGridAreaOfInterest::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildGrid();
}

void AGridAreaOfInterest::Destroyed()
{
	for (AAreaGridCell* cell : Cells)
	{
		cell->Destroy();
	}
}


void AGridAreaOfInterest::BuildGrid()
{

	for (AAreaGridCell* cell : Cells)
	{
		if (cell->IsValidLowLevel())
		{
			cell->Destroy();
		}
	}
	Cells.Empty();

	WidthCells = FMath::FloorToInt((2 * BoxExtent.X) / CellWidth);
	HeightCells = FMath::FloorToInt((2 * BoxExtent.Y) / CellHeight);
	TotalCells = HeightCells * WidthCells;

	if (!CellsClass->IsValidLowLevel())
		return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
		return;

	for (int32 i = 0; i < WidthCells; i++)
	{
		for (int32 j = 0; j < HeightCells; j++)
		{
			float x = CellWidth / 2 + BoxExtent.X - CellWidth * (i + 1);
			float y = CellHeight / 2 + BoxExtent.Y - CellHeight * (j + 1);
			float z = 0;
			FVector localPosition = FVector(x, y, z);
			FVector extent = FVector(CellWidth / 2, CellHeight / 2, BoxExtent.Z);
			FNavLocation projectLocation;
			FTransform localTransform = FTransform(localPosition);
			AAreaGridCell* cell = Cast<AAreaGridCell>(GetWorld()->SpawnActor(CellsClass));
			cell->Initialize(extent, this);
			cell->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			cell->SetActorRelativeLocation(localPosition);
			if (NavSys->ProjectPointToNavigation(cell->GetActorLocation(), projectLocation))
				Cells.Add(Cast<AAreaGridCell>(cell));
			else
			{
				cell->Destroy();
				TotalCells--;
			}

		}
	}
}

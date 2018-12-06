// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PointsOfInterest/AreaOfInterest.h"
#include "PointsOfInterest//AreaGridCell.h"
#include "GridAreaOfInterest.generated.h"

/**
 *
 */

UCLASS()
class UTILITYAIPLUGIN_API AGridAreaOfInterest : public AAreaOfInterest
{
	GENERATED_BODY()

public:
	AGridAreaOfInterest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CellHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CellWidth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 WidthCells;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 HeightCells;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 TotalCells;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AAreaGridCell> CellsClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<AAreaGridCell*> Cells;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TMap<APawn*, AAreaGridCell*> UserCell;

	UFUNCTION(BlueprintCallable, Category = "Navigation")
		void AppointPawnToGridCell(APawn* pawn, AAreaGridCell* Cell);
	UFUNCTION(BlueprintCallable, Category = "Navigation")
		void GetCurrentGridCell(APawn* pawn, AAreaGridCell*& Cell);

	UFUNCTION()
		void ProcessSellBeginOverlap(AAreaGridCell* Cell, AActor* Actor);
	UFUNCTION()
		void ProcessCellEndOverlap(AAreaGridCell* Cell, AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cells")
		void OnCellBeginOverlap(AAreaGridCell* Cell, AActor* Actor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Cells")
		void OnCellEndOverlap(AAreaGridCell* Cell, AActor* Actor);

private:
		void PostEditChangeProperty(struct FPropertyChangedEvent& e);
		void OnConstruction(const FTransform& Transform);
	UFUNCTION()
		void Destroyed() override;
	UFUNCTION()
		void BuildGrid();
};

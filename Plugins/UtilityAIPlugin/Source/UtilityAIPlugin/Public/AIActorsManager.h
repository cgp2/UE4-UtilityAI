// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include "UtilityAIPlugin/PointsOfInterest/PointOfInterestComponent.h"
#include "UtilityAIPlugin/PointsOfInterest/AreaOfInterest.h"
#include "AIActorsManager.generated.h"

/**
 *
 */
UCLASS()
class UTILITYAIPLUGIN_API UAIActorsManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TMap<FGuid, APawn*> AIPawns;
	UPROPERTY()
		TMap<FGuid, UPointOfInterestComponent*>  PointsOfInterest;
	UPROPERTY()
		TMap<FGuid, AAreaOfInterest*> AreasOfInterest;

	UPROPERTY()
		bool IsInitialized;


		bool Initialize(UWorld* World);

		void Destroy(UWorld* World);
		APawn* GetAIPawn(const FGuid ID);

		UPointOfInterestComponent* GetPointOfInterest(const FGuid ID);

		AAreaOfInterest* GetAreaOfInterest(const FGuid ID);
		TArray<UPointOfInterestComponent*> GetPointsOfInterest();
		TArray<APawn*> GetAIPawns();
		TArray<AAreaOfInterest*> GetAreasOfInterest();
		bool AddAIPawn(APawn* pawn);
		bool RemoveAIPawn(APawn* pawn);
		bool RemovePointOfInterest(UPointOfInterestComponent* poi);
		bool RemoveAreaOfInterest(AAreaOfInterest* area);
		bool AddPointOfInterest(UPointOfInterestComponent* poi);
	
		bool AddArea(AAreaOfInterest* area);
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "AIActorsManager.h"

bool UAIActorsManager::Initialize(UWorld* world)
{
	AIPawns.Empty();
	AreasOfInterest.Empty();
	PointsOfInterest.Empty();

	IsInitialized = true;
	return true;
}

void UAIActorsManager::Destroy(UWorld* world)
{
	IsInitialized = false;
	AIPawns.Empty();
	AreasOfInterest.Empty();
	PointsOfInterest.Empty();
}

APawn* UAIActorsManager::GetAIPawn(const FGuid ID)
{
	if (AIPawns.Contains(ID))
	{
		APawn* pawn = *AIPawns.Find(ID);
		if (pawn)
			return pawn;
		else
			return nullptr;
	}
	else
	{
		return nullptr;
	}
}

UPointOfInterestComponent* UAIActorsManager::GetPointOfInterest(const FGuid ID)
{
	if (PointsOfInterest.Contains(ID))
	{
		UPointOfInterestComponent* poi = *PointsOfInterest.Find(ID);
		if (poi)
			return poi;
		else
			return nullptr;
	}
	else
	{
		return nullptr;
	}
}

AAreaOfInterest* UAIActorsManager::GetAreaOfInterest(const FGuid ID)
{
	if (AreasOfInterest.Contains(ID))
	{
		AAreaOfInterest* area = *AreasOfInterest.Find(ID);
		if (area)
			return area;
		else
			return nullptr;
	}
	else
	{
		return nullptr;
	}
}

TArray<UPointOfInterestComponent*> UAIActorsManager::GetPointsOfInterest()
{
	TArray<UPointOfInterestComponent*> points;
	PointsOfInterest.GenerateValueArray(points);
	return points;
}

TArray<APawn*> UAIActorsManager::GetAIPawns()
{
	TArray<APawn*> pawns;
	AIPawns.GenerateValueArray(pawns);
	return pawns;
}

TArray<AAreaOfInterest*> UAIActorsManager::GetAreasOfInterest()
{
	TArray<AAreaOfInterest*> areas;
	AreasOfInterest.GenerateValueArray(areas);
	return areas;
}

bool UAIActorsManager::AddAIPawn(APawn* pawn)
{
	if (pawn)
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->GetController());
		if (controller)
		{
			if (AIPawns.Num() > 0)
			{
				if (!AIPawns.Contains(controller->ID))
				{
					AIPawns.Add(controller->ID, pawn);
					return true;
				}
			}
			else
			{
				AIPawns.Add(controller->ID, pawn);
				return true;
			}

		}
	}
	return false;
}

bool UAIActorsManager::RemoveAIPawn(APawn* pawn)
{
	if (pawn->IsValidLowLevel())
	{
		AUtilityAIController* controller = Cast<AUtilityAIController>(pawn->GetController());
		if (AIPawns.Contains(controller->ID))
		{
			AIPawns.Remove(controller->ID);
			return true;
		}
	}
	return false;
}

bool UAIActorsManager::RemovePointOfInterest(UPointOfInterestComponent* poi)
{
	if (poi->IsValidLowLevel())
	{
		if (PointsOfInterest.Contains(poi->ID))
		{
			PointsOfInterest.Remove(poi->ID);
			return true;
		}
	}
	return false;
}

bool UAIActorsManager::RemoveAreaOfInterest(AAreaOfInterest* area)
{
	if (area->IsValidLowLevel())
	{
		if (AreasOfInterest.Contains(area->ID))
		{
			AreasOfInterest.Remove(area->ID);
			return true;
		}
	}
	return false;
}

bool UAIActorsManager::AddPointOfInterest(UPointOfInterestComponent* poi)
{
	if (poi)
	{
		if (PointsOfInterest.Num() > 0)
		{
			if (!PointsOfInterest.Contains(poi->ID))
			{
				PointsOfInterest.Add(poi->ID, poi);
				return true;
			}
		}
		else
		{
			PointsOfInterest.Add(poi->ID, poi);
			return true;
		}
	}
	return false;
}

bool UAIActorsManager::AddArea(AAreaOfInterest* area)
{
	if (area)
	{
		if (AreasOfInterest.Num() > 0)
		{
			if (!AreasOfInterest.Contains(area->ID))
			{
				AreasOfInterest.Add(area->ID, area);
				return true;
			}
		}
		else
		{
			AreasOfInterest.Add(area->ID, area);
			return true;
		}
	}
	return false;
}

// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIActorsManager.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"

class FUtilityAIPluginModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnWorldCreated(UWorld* World, const UWorld::InitializationValues IVS);
	void OnWorldCleanUp(UWorld* World);

	UAIActorsManager* AIManager;

	bool IsDebug;

	void StartAIDebugMode();
	void EndAIDebugMode();

	static inline FUtilityAIPluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FUtilityAIPluginModule>("UtilityAIPlugin");
	}

private:
	
};

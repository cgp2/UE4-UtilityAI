// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UtilityAIPlugin.h"

//#define LOCTEXT_NAMESPACE "FUtilityAIPluginModule"

void FUtilityAIPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IsDebug = true;
	AIManager = NewObject<UAIActorsManager>();
	
	FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FUtilityAIPluginModule::OnWorldCreated);
	
	FWorldDelegates::FWorldEvent::FDelegate OnWorldCleanupDelegate;
	OnWorldCleanupDelegate = FWorldDelegates::FWorldEvent::FDelegate::CreateRaw(this, &FUtilityAIPluginModule::OnWorldCleanUp);
	FDelegateHandle OnWorldCleanupDelegateHandle = FWorldDelegates::OnPreWorldFinishDestroy.Add(OnWorldCleanupDelegate);
}

void FUtilityAIPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FUtilityAIPluginModule::OnWorldCreated(UWorld* World, const UWorld::InitializationValues IVS) 
{

	//AIManager = NewObject<UAIActorsManager>();
	if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
	{
		if (AIManager)
		{
			if (!AIManager->IsInitialized)
				AIManager->Initialize(World);
		}
	}
}

void FUtilityAIPluginModule::OnWorldCleanUp(UWorld* World)
{
	if(World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
	{
		if (AIManager)
		{
			if (AIManager->IsInitialized)
			{
				AIManager->Destroy(World);
			}
		}
	}
}



void FUtilityAIPluginModule::StartAIDebugMode()
{

}

void FUtilityAIPluginModule::EndAIDebugMode()
{

}

//#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUtilityAIPluginModule, UtilityAIPlugin)
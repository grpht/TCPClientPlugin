// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPClientPlugin.h"

#define LOCTEXT_NAMESPACE "FTCPClientPluginModule"

void FTCPClientPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FTCPClientPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTCPClientPluginModule, TCPClientPlugin)
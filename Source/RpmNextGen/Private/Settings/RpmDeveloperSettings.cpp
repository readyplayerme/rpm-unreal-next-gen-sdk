// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/RpmDeveloperSettings.h"

URpmDeveloperSettings::URpmDeveloperSettings() : ApiBaseUrl(TEXT("https://api.readyplayer.me")), ApiBaseAuthUrl(TEXT("https://readyplayer.me/api/auth"))
{
	LoadConfig();
}

void URpmDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

void URpmDeveloperSettings::PreSave(const ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);

	if(ApiKey.IsEmpty() && ApiProxyUrl.IsEmpty() && !ApplicationId.IsEmpty())
	{
		return;
	}
	
	// Ensure settings are saved before the build
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}

#if WITH_EDITOR
void URpmDeveloperSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}
#endif // WITH_EDITOR


void URpmDeveloperSettings::SetupDemoAccount()
{
	ApplicationId = DemoAppId;
	ApiProxyUrl = DemoProxyUrl;
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}

void URpmDeveloperSettings::Reset()
{
	if(ApplicationId == DemoAppId)
	{
		ApplicationId = TEXT("");
	}
	if(ApiProxyUrl == DemoProxyUrl)
	{
		ApiProxyUrl = TEXT("");
	}
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}

FString URpmDeveloperSettings::GetApiBaseUrl() const
{
	return ApiProxyUrl.IsEmpty() ? ApiBaseUrl : ApiProxyUrl;
}

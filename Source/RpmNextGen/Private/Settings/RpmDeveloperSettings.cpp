// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/RpmDeveloperSettings.h"

URpmDeveloperSettings::URpmDeveloperSettings()
{
	LoadConfig();
	ApiBaseUrl = TEXT("https://api.readyplayer.me");
	ApiBaseAuthUrl = TEXT("https://readyplayer.me/api/auth");
}

void URpmDeveloperSettings::SetupDemoAccount()
{
	ApplicationId = DemoAppId;
	ApiProxyUrl = DemoProxyUrl;
	SaveConfig();
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
	
	SaveConfig();
}

FString URpmDeveloperSettings::GetApiBaseUrl() const
{
	return ApiProxyUrl.IsEmpty() ? ApiBaseUrl : ApiProxyUrl;
}

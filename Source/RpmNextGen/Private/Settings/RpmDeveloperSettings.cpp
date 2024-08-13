// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/RpmDeveloperSettings.h"

URpmDeveloperSettings::URpmDeveloperSettings()
{
	ApiBaseUrl = TEXT("https://api.readyplayer.me");
	ApiBaseAuthUrl = TEXT("https://readyplayer.me/api/auth");
	ApplicationId = TEXT("");
	ApiKey = TEXT("");
	ApiProxyUrl = TEXT("");
}

void URpmDeveloperSettings::SetupDemoAccount()
{
	ApplicationId = DemoAppId;
	ApiProxyUrl = DemoProxyUrl;
	this->SaveConfig();
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
	this->SaveConfig();
}

FString URpmDeveloperSettings::GetApiBaseUrl()
{
	return ApiProxyUrl.IsEmpty() ? ApiBaseUrl : ApiProxyUrl;
}

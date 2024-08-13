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

void URpmDeveloperSettings::SetupGuestUser()
{
	ApplicationId = TEXT("665e05a50c62c921e5a6ab84");
	ApiProxyUrl = TEXT("https://api.readyplayer.me/demo");
	
}

void URpmDeveloperSettings::Reset()
{
	if(ApplicationId == TEXT("665e05a50c62c921e5a6ab84"))
	{
		ApplicationId = TEXT("");

	}
	if(ApiProxyUrl == TEXT("https://api.readyplayer.me/demo"))
	{
		ApiProxyUrl = TEXT("");
	}
	this->SaveConfig();
}

FString URpmDeveloperSettings::GetApiBaseUrl()
{
	return ApiProxyUrl.IsEmpty() ? ApiBaseUrl : ApiProxyUrl;
}

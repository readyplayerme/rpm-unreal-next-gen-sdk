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

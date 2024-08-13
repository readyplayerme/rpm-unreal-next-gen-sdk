// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RpmDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Ready Player Me"))
class RPMNEXTGEN_API URpmDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URpmDeveloperSettings();

	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApiBaseAuthUrl;
	
	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApplicationId;

	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApiKey;
	
	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApiProxyUrl;

	void SetupDemoAccount();
	void Reset();
	FString GetApiBaseUrl();

private:	
	FString ApiBaseUrl;
	const FString DemoAppId = TEXT("665e05a50c62c921e5a6ab84");
	const FString DemoProxyUrl = TEXT("https://api.readyplayer.me/demo");
};

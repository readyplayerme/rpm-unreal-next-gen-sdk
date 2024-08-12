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
	FString ApiBaseUrl;

	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApiBaseAuthUrl;
	
	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApplicationId;

	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApiKey;
	
	UPROPERTY(EditAnywhere, Config, Category = "API")
	FString ApiProxyUrl;

	void SetupGuestUser();
	
};

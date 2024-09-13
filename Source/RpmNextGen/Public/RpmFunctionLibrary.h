// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utilities/ConnectionManager.h"
#include "RpmFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAssetIdFetched, FString, AssetId);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnConnectionStatusRefreshedDelegate, bool, bIsConnected);

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = "ReadyPlayerMe", meta = (WorldContext = "WorldContextObject"))
	static void FetchFirstAssetId(UObject* WorldContextObject, const FString& AssetType, FOnAssetIdFetched OnAssetIdFetched);

	UFUNCTION(BlueprintCallable, Category = "ReadyPlayerMe/Network")
	static bool IsInternetConnected();

	UFUNCTION(BlueprintCallable, Category = "ReadyPlayerMe/Network")
	static void CheckInternetConnection(const FOnConnectionStatusRefreshedDelegate& OnConnectionStatusRefreshed);
};

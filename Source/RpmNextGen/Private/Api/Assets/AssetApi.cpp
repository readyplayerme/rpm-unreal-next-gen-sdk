﻿#include "Api/Assets/AssetApi.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"

FAssetApi::FAssetApi()
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	ApiBaseUrl = Settings->ApiBaseUrl;
	OnApiResponse.BindRaw(this, &FAssetApi::HandleListAssetResponse);
}


void FAssetApi::ListAssetsAsync(const FAssetListRequest& Request)
{
	FString QueryString = Request.BuildQueryString();
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets%s"), *ApiBaseUrl, *QueryString);
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = Url;
	ApiRequest.Method = GET;
	UE_LOG(LogTemp, Warning, TEXT("Try request from url %s"), *Url);

	DispatchRawWithAuth(ApiRequest);
}

void FAssetApi::HandleListAssetResponse(FString Response, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		FAssetListResponse AssetListResponse = FAssetListResponse();
		if(FJsonObjectConverter::JsonObjectStringToUStruct(Response, &AssetListResponse, 0, 0))
		{
			OnListAssetsResponse.ExecuteIfBound(AssetListResponse, true);
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("API Response was unsuccessful or failed to parse"));
	}
	OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
}

void FAssetApi::HandleListAssetTypeResponse(FString Response, bool bWasSuccessful)
{
}

﻿#include "Api/Assets/AssetApi.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"

FAssetApi::FAssetApi()
{
	OnApiResponse.BindRaw(this, &FAssetApi::HandleListAssetResponse);
}


void FAssetApi::ListAssetsAsync(const FAssetListRequest& Request)
{
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	ApiBaseUrl = RpmSettings->GetApiBaseUrl();
	if(RpmSettings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Application ID is empty"));
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	FString QueryString = Request.BuildQueryString();
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets%s"), *ApiBaseUrl, *QueryString);
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = Url;
	ApiRequest.Method = GET;
	
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
		UE_LOG(LogTemp, Error, TEXT("Failed to parse API response"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("API Response was unsuccessful"));
	}
	OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
}

void FAssetApi::HandleListAssetTypeResponse(FString Response, bool bWasSuccessful)
{
}

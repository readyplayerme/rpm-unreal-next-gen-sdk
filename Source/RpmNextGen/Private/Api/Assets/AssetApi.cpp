#include "Api/Assets/AssetApi.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"

struct FAssetTypeListRequest;
const FString FAssetApi::BaseModelType = TEXT("baseModel");

FAssetApi::FAssetApi()
{
	OnApiResponse.BindRaw(this, &FAssetApi::HandleListAssetResponse);
	const URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();

	if(Settings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Application ID is empty. Please set the Application ID in the Ready Player Me Developer Settings"));
	}
}

void FAssetApi::ListAssetsAsync(const FAssetListRequest& Request)
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	ApiBaseUrl = Settings->GetApiBaseUrl();

	if(Settings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Application ID is empty"));
		OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
		return;
	}
	FString QueryString = Request.BuildQueryString();
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets/types%s"), *ApiBaseUrl, *QueryString);
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = Url;
	ApiRequest.Method = GET;
	
	DispatchRawWithAuth(ApiRequest);
}

void FAssetApi::ListAssetTypesAsync(const FAssetTypeListRequest& Request)
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	ApiBaseUrl = Settings->GetApiBaseUrl();

	if(Settings->ApplicationId.IsEmpty())
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

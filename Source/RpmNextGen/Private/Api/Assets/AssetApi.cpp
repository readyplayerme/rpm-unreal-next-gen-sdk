#include "Api/Assets/AssetApi.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"

struct FAssetTypeListRequest;
const FString FAssetApi::BaseModelType = TEXT("baseModel");

FAssetApi::FAssetApi()
{
	OnApiResponse.BindRaw(this, &FAssetApi::HandleListAssetResponse);

	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();

	if(Settings->ApplicationId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Application ID is empty. Please set the Application ID in the Ready Player Me Developer Settings"));
	}
	
	if(!Settings->ApiKey.IsEmpty())
	{
		SetAuthenticationStrategy(new FApiKeyAuthStrategy());
	}
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
	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets/types%s"), *ApiBaseUrl, *QueryString);
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = Url;
	ApiRequest.Method = GET;
	
	DispatchRawWithAuth(ApiRequest);
}

void FAssetApi::HandleListAssetResponse(FString Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        #if ENGINE_MAJOR_VERSION < 5 || ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 1 // Manual parsing for Unreal Engine 5.0 and earlier
    	
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // Check if the "data" field is an array
            const TArray<TSharedPtr<FJsonValue>>* DataArray;
            if (JsonObject->TryGetArrayField(TEXT("data"), DataArray))
            {
                if (DataArray->Num() > 0)
                {
                    // Check the type of the first element to determine the response type
                    const TSharedPtr<FJsonValue>& FirstElement = (*DataArray)[0];

                    if (FirstElement->Type == EJson::Object)
                    {
                        // Assume this is an FAssetListResponse
                        FAssetListResponse AssetListResponse;
                        if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &AssetListResponse, 0, 0))
                        {
                            OnListAssetsResponse.ExecuteIfBound(AssetListResponse, true);
                            return;
                        }
                    }
                    else if (FirstElement->Type == EJson::String)
                    {
                        // Assume this is an FAssetTypeListResponse
                        FAssetTypeListResponse AssetTypeListResponse;
                        if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &AssetTypeListResponse, 0, 0))
                        {
                            OnListAssetTypeResponse.ExecuteIfBound(AssetTypeListResponse, true);
                            return;
                        }
                    }
                }
            }
        }

        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON into known structs from response: %s"), *Response);

        #else

        // Use EStructJsonFlags::SkipMissingProperties for Unreal Engine 5.1 and later
        FAssetListResponse AssetListResponse = FAssetListResponse();
        FAssetTypeListResponse AssetTypeListResponse = FAssetTypeListResponse();
        if (FJsonObjectConverter::JsonObjectStringToUStruct(Response, &AssetListResponse, 0, EStructJsonFlags::SkipMissingProperties))
        {
            OnListAssetsResponse.ExecuteIfBound(AssetListResponse, true);
            return;
        }
        if (FJsonObjectConverter::JsonObjectStringToUStruct(Response, &AssetTypeListResponse, 0, EStructJsonFlags::SkipMissingProperties))
        {
            OnListAssetTypeResponse.ExecuteIfBound(AssetTypeListResponse, true);
            return;
        }

        UE_LOG(LogTemp, Error, TEXT("Failed to parse API from response %s"), *Response);

        #endif
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("API Response was unsuccessful"));
    }

    // If all parsing attempts fail, execute with default/empty responses
    OnListAssetsResponse.ExecuteIfBound(FAssetListResponse(), false);
    OnListAssetTypeResponse.ExecuteIfBound(FAssetTypeListResponse(), false);
}

void FAssetApi::HandleListAssetTypeResponse(FString Response, bool bWasSuccessful)
{
	
}


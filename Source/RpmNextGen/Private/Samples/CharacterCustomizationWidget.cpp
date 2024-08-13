#include "Samples/CharacterCustomizationWidget.h"
#include "Samples/AssetButtonWidget.h"
#include "HttpModule.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Characters/CharacterApi.h"
#include "Interfaces/IHttpResponse.h"

void UCharacterCustomizationWidget::NativeConstruct()
{
    Super::NativeConstruct();
    URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
    ApplicationID = Settings->ApplicationId;
    InitializeCustomizationOptions();
}

void UCharacterCustomizationWidget::InitializeCustomizationOptions()
{
    URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
    FString ApiBaseUrl = Settings->GetApiBaseUrl();
    
    // Setup request and parameters
    FAssetListRequest Request = FAssetListRequest();
    FAssetListQueryParams Params;
    Params.ApplicationId = ApplicationID;
    Params.ExcludeTypes = "baseModel";
    Request.Params = Params;
    FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets%s"), *ApiBaseUrl, *Request.BuildQueryString());

    // TODO replace this with use of AssetApi class
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb("GET");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetHeader("X-API-KEY", Settings->ApiKey);
    HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (bWasSuccessful)
        {
            // Parse the response to fill Assets
            FAssetListResponse AssetListResponse = FAssetListResponse();
            if (FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &AssetListResponse, 0, 0))
            {
                OnAssetsFetched(AssetListResponse, true);
                return;
            }
        }
        else
        {
            // Handle error
        }
        OnAssetsFetched(FAssetListResponse(), false);  // Call with empty response on failure
    });
    HttpRequest->ProcessRequest();
    
    UE_LOG(LogTemp, Warning, TEXT("Made request"));
}

void UCharacterCustomizationWidget::OnAssetsFetched(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnAssetsFetched called. Success: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));

    if (AssetListResponse.Data.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Number of assets fetched: %d"), AssetListResponse.Data.Num());
        AssetDataArray = AssetListResponse.Data;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No assets fetched"));
    }
    // Broadcast the delegate to notify Blueprints
    OnAssetsFetchedDelegate.Broadcast(bWasSuccessful, AssetDataArray);
}

void UCharacterCustomizationWidget::PopulateBoxWithFilter(UPanelWidget* ParentBox, const FString& AssetType)
{
    if (!ParentBox)
    {
        UE_LOG(LogTemp, Error, TEXT("ParentBox is null"));
        return;
    }

    for (const FAsset& AssetData : AssetDataArray)
    {
        if (AssetData.Type == AssetType)
        {
            CreateAssetWidget(AssetData, ParentBox);
        }
    }
}

void UCharacterCustomizationWidget::PopulateBox(UPanelWidget* ParentBox)
{
    if (!ParentBox)
    {
        UE_LOG(LogTemp, Error, TEXT("ParentBox is null"));
        return;
    }

    for (const FAsset& AssetData : AssetDataArray)
    {
        CreateAssetWidget(AssetData, ParentBox);
    }
}

void UCharacterCustomizationWidget::ClearBox(UPanelWidget* ParentBox)
{
    if (!ParentBox)
    {
        UE_LOG(LogTemp, Error, TEXT("ParentBox is null"));
        return;
    }

    ParentBox->ClearChildren();
}

void UCharacterCustomizationWidget::UpdateCustomizationOptions(UPanelWidget* ParentBox)
{
    ClearBox(ParentBox);
    InitializeCustomizationOptions();
    PopulateBox(ParentBox);
}

void UCharacterCustomizationWidget::CreateAssetWidget(const FAsset& AssetData, UPanelWidget* ParentBox)
{
    if (!ParentBox)
    {
        UE_LOG(LogTemp, Error, TEXT("ParentBox is null"));
        return;
    }

    if (!AssetButtonWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AssetButtonWidgetClass is null"));
        return;
    }

    UAssetButtonWidget* AssetButton = CreateWidget<UAssetButtonWidget>(this, AssetButtonWidgetClass);
    if (AssetButton)
    {
        AssetButton->InitializeButton(AssetData, ImageSize);
        AssetButton->OnAssetButtonClicked.AddDynamic(this, &UCharacterCustomizationWidget::OnAssetButtonClicked);
        ParentBox->AddChild(AssetButton);
    }
}

void UCharacterCustomizationWidget::OnAssetButtonClicked(const FAsset& AssetData)
{
    FString GlbUrl = AssetData.GlbUrl;
    UE_LOG(LogTemp, Log, TEXT("Asset Button Clicked: %s"), *GlbUrl);
    OnAssetButtonSelected.Broadcast(AssetData);
}

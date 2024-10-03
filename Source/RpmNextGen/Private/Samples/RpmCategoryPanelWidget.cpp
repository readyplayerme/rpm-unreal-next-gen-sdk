// Fill out your copyright notice in the Description page of Project Settings.

#include "Samples/RpmCategoryPanelWidget.h"

#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Interfaces/IHttpResponse.h"
#include "Samples/RpmCategoryButtonWidget.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmDeveloperSettings;

void URpmCategoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("URpmCategoryPanelWidget Already initialized"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Initiizing URpmCategoryPanelWidget"));
	bIsInitialized = true;
	AssetApi = MakeShared<FAssetApi>();
	AssetApi->OnListAssetTypeResponse.BindUObject(this, &URpmCategoryPanelWidget::AssetTypesLoaded);
	AssetButtons = TArray<TSubclassOf<URpmCategoryButtonWidget>>();
}

void URpmCategoryPanelWidget::UpdateSelectedButton(URpmCategoryButtonWidget* CategoryButton)
{
	if(SelectedCategoryButton && SelectedCategoryButton != CategoryButton)
	{
		SelectedCategoryButton->SetSelected(false);
	}
	SelectedCategoryButton = CategoryButton;
}

void URpmCategoryPanelWidget::OnProcessResponse(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bWasSuccessful, FApiRequest* ApiRequest)
{
	if(bWasSuccessful && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("RpmCategory Widget from URL %s SUCCESS"), *HttpRequest->GetURL());
		return;
	}
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("RpmCategory Widget from URL %s : FAILED"), *HttpRequest->GetURL());

}

// void URpmCategoryPanelWidget::LoadAndCreateButtons()
// {
// 	// make manual HTTPRequest
// 	TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
// 	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
// 	FAssetTypeListRequest AssetTypeListRequest;
// 	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
// 	QueryParams.ApplicationId = Settings->ApplicationId;
// 	AssetTypeListRequest.Params = QueryParams;
// 	
// 	auto ApiBaseUrl = Settings->GetApiBaseUrl();
// 	FString QueryString = AssetTypeListRequest.BuildQueryString();
// 	const FString Url = FString::Printf(TEXT("%s/v1/phoenix-assets/types%s"), *ApiBaseUrl, *QueryString);
// 	FApiRequest ApiRequest = FApiRequest();
// 	ApiRequest.Url = Url;
// 	ApiRequest.Method = GET;
// 	Request->SetURL(Url);
// 	Request->SetVerb(ApiRequest.GetVerb());
// 	Request->SetTimeout(10);
// 	FString Headers;
// 	Request->SetHeader(TEXT("X-API-KEY"), Settings->ApiKey);
// 	for (const auto& Header : ApiRequest.Headers)
// 	{
// 		Request->SetHeader(Header.Key, Header.Value);
// 		Headers.Append(FString::Printf(TEXT("%s: %s\n"), *Header.Key, *Header.Value));
// 	}
//
// 	if (!ApiRequest.Payload.IsEmpty() && ApiRequest.Method != ERequestMethod::GET)
// 	{
// 		Request->SetContentAsString(ApiRequest.Payload);
// 	}
// 	Request->OnProcessRequestComplete().BindUObject(this, &URpmCategoryPanelWidget::OnProcessResponse, &ApiRequest);
// 	UE_LOG( LogTemp, Warning, TEXT("Process request to url %s with headers %s"), *Request->GetURL(), *Headers);
// 	Request->ProcessRequest();
// }

void URpmCategoryPanelWidget::LoadAndCreateButtons()
{
	UE_LOG(LogTemp, Warning, TEXT("Creating buttons on URpmCategoryPanelWidget"));
	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
	FAssetTypeListRequest AssetTypeListRequest;
	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	AssetTypeListRequest.Params = QueryParams;
	AssetApi->ListAssetTypesAsync(AssetTypeListRequest);
}

void URpmCategoryPanelWidget::OnCategoryButtonClicked(URpmCategoryButtonWidget* CategoryButton)
{
	UpdateSelectedButton(CategoryButton);
	OnCategorySelected.Broadcast(CategoryButton->AssetCategoryName);
}

void URpmCategoryPanelWidget::CreateButton(const FString& AssetType)
{
	if (UWorld* World = GetWorld())
	{
		URpmCategoryButtonWidget* CategoryButton = CreateWidget<URpmCategoryButtonWidget>(World, CategoryButtonBlueprint);
		UTexture2D* ButtonTexture = nullptr;
		const auto CleanAssetType = AssetType.Replace(TEXT(" "), TEXT("-"));
		if(UObject* LoadedAsset = StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString::Printf(TEXT("/RpmNextGen/Samples/Icons/T-rpm-%s"), *CleanAssetType)))
		{
			ButtonTexture = Cast<UTexture2D>(LoadedAsset);
		}
		if (CategoryButton)
		{

			USizeBox* ButtonSizeBox = NewObject<USizeBox>(this);
			if (ButtonSizeBox && ButtonContainer)
			{
				ButtonSizeBox->SetWidthOverride(ButtonSize.X);
				ButtonSizeBox->SetHeightOverride(ButtonSize.Y);
				ButtonSizeBox->AddChild(CategoryButton);
				ButtonContainer->AddChild(ButtonSizeBox);
				CategoryButton->InitializeButton(AssetType, ButtonTexture, ButtonSize);
				CategoryButton->OnCategoryClicked.AddDynamic(this, &URpmCategoryPanelWidget::OnCategoryButtonClicked);
				AssetButtons.Add(CategoryButton->GetClass());
				return;
			}
			UE_LOG(LogTemp, Error, TEXT("Failed to Load %s button"), *AssetType);
		}
	}
}

void URpmCategoryPanelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void URpmCategoryPanelWidget::AssetTypesLoaded(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful)
{	
	if(bWasSuccessful && ButtonContainer)
	{
		ButtonContainer->ClearChildren();

		for (const FString& AssetType : AssetTypeListResponse.Data)
		{
			CreateButton(AssetType);
		}
		OnCategoriesLoaded.Broadcast(AssetTypeListResponse.Data);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load asset types"));
	OnCategoriesLoaded.Broadcast(TArray<FString>());
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLoader.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Auth/DeveloperAccountApi.h"
#include "Interfaces/IHttpRequest.h"
#include "Widgets/SCompoundWidget.h"
#include "Containers/Map.h"

class URpmSettings;
class UDeveloperAuthApi;
class SEditableTextBox;
/**
 * 
 */
class RPMNEXTGENEDITOR_API SRpmDeveloperLoginWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRpmDeveloperLoginWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	FReply OnLoginClicked();
	void GetOrgList();
	FReply OnUseDemoAccountClicked();
	FReply OnLogoutClicked();
	

	void LoadBaseModelList();

	void HandleLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleOrganizationListResponse(const FOrganizationListResponse& Response, bool bWasSuccessful);

	void HandleApplicationListResponse(const FApplicationListResponse& Response, bool bWasSuccessful);
	void HandleBaseModelListResponse(const FAssetListResponse& Response, bool bWasSuccessful);
	void OnLoadStyleClicked(const FString& StyleId);
	EVisibility GetLoginViewVisibility() const;
	EVisibility GetLoggedInViewVisibility() const;
	
	FText GetWelcomeText() const;
	
	void SetLoggedInState(const bool IsLoggedIn);
	void PopulateComboBoxItems(const TArray<FString>& Items);
	
	const URpmSettings* Settings;
	TSharedPtr<SEditableTextBox> EmailTextBox;
	TSharedPtr<SEditableTextBox> PasswordTextBox;

	TArray<TSharedPtr<FString>> ComboBoxItems;
	TSharedPtr<FString> SelectedComboBoxItem;

	void OnComboBoxSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo);
	FText GetSelectedComboBoxItemText() const;
	
	bool bIsLoggedIn = false;
	FString UserName;
	TUniquePtr<FAssetApi> AssetApi;
	TUniquePtr<FDeveloperAccountApi> DeveloperApi;
	static constexpr const TCHAR* CacheKeyEmail = TEXT("Email");

	TSharedPtr<SVerticalBox> ContentBox;

	TMap<FString, FAsset> CharacterStyleAssets;
	void AddCharacterStyle(const FAsset& StyleAsset);
	void DownloadImage(const FString& Url, TFunction<void(UTexture2D*)> Callback);
	UTexture2D* CreateTextureFromImageData(const TArray<uint8>& ImageData);
	static void SetImageFromTexture(UTexture2D* Texture, const TSharedPtr<SImage>& ImageWidget);
	bool bIsInitialized = false;
	FEditorAssetLoader AssetLoader;
	void Initialize();
};

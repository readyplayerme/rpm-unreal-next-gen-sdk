// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLoader.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "DeveloperAccounts/DeveloperAccountApi.h"
#include "Widgets/SCompoundWidget.h"
#include "Containers/Map.h"

struct FDeveloperLoginResponse;
class FDeveloperAuthApi;
class URpmDeveloperSettings;
class UDeveloperAuthApi;
class SEditableTextBox;

/**
 * 
 */
class RPMNEXTGENEDITOR_API SRpmDeveloperLoginWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRpmDeveloperLoginWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SRpmDeveloperLoginWidget() override;

private:
	TSharedPtr<SVerticalBox> ContentBox;
	TSharedPtr<SEditableTextBox> EmailTextBox;
	TSharedPtr<SEditableTextBox> PasswordTextBox;
	TSharedPtr<STextBlock> SelectedApplicationTextBlock;
	TSharedPtr<FString> SelectedComboBoxItem;
	TArray<TSharedPtr<FString>> ComboBoxItems;
	TArray<UTexture2D*> CharacterStyleTextures;
	TMap<FString, FAsset> CharacterStyleAssets;

	EVisibility GetLoginViewVisibility() const;
	EVisibility GetLoggedInViewVisibility() const;

	FEditorAssetLoader AssetLoader;
	TUniquePtr<FAssetApi> AssetApi;
	TUniquePtr<FDeveloperAccountApi> DeveloperAccountApi;
	TUniquePtr<FDeveloperAuthApi> DeveloperAuthApi;
	static constexpr const TCHAR* CacheKeyEmail = TEXT("Email");
	bool bIsLoggedIn = false;
	bool bIsInitialized = false;
	FString UserName;
	TArray<FApplication> UserApplications;
	FText GetWelcomeText() const;
	FString DemoUserName = TEXT("Guest user");
	FText GetSelectedComboBoxItemText() const;

	FReply OnLoginClicked();
	FReply OnUseDemoAccountClicked();
	FReply OnLogoutClicked();

	void Initialize();
	void GetOrgList();
	void ClearLoadedCharacterModelImages();
	void LoadBaseModelList();
	void HandleLoginResponse(const FDeveloperLoginResponse& Response, bool bWasSuccessful);
	void HandleOrganizationListResponse(const FOrganizationListResponse& Response, bool bWasSuccessful);
	void HandleApplicationListResponse(const FApplicationListResponse& Response, bool bWasSuccessful);
	void HandleBaseModelListResponse(const FAssetListResponse& Response, bool bWasSuccessful);
	void OnLoadStyleClicked(const FString& StyleId);
	void SetLoggedInState(const bool IsLoggedIn);
	void PopulateComboBoxItems(const TArray<FString>& Items, const FString ActiveItem);
	void OnComboBoxSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo);
	void AddCharacterStyle(const FAsset& StyleAsset);
};

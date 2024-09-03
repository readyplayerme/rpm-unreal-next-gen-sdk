#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FCacheGenerator;

class SCacheEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCacheEditorWidget) {}
	SLATE_END_ARGS()
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	TUniquePtr<FCacheGenerator> CacheGenerator;
	// Callback functions for your buttons
	FReply OnGenerateOfflineCacheClicked();
	FReply OnExtractCacheClicked();
	FReply OnOpenLocalCacheFolderClicked();
	FReply OnDownloadRemoteCacheClicked();

	void OnFetchCacheDataComplete(bool bWasSuccessful);
	void OnDownloadRemoteCacheComplete(bool bWasSuccessful);
	void OnGenerateLocalCacheCompleted(bool bWasSuccessful);
	
	TOptional<int32> GetItemsPerCategory() const
	{
		return ItemsPerCategory;
	}
	
	void OnItemsPerCategoryChanged(int32 NewValue)
	{
		ItemsPerCategory = NewValue;
	}

	void OnCacheUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType)
	{
		CacheUrl = NewText.ToString();
	}

	// Slider value handling
	float ItemsPerCategory = 10.0f;
	void OnItemsPerCategoryChanged(float NewValue);

	// Cache URL handling
	FString CacheUrl;
	void OnCacheUrlChanged(const FText& NewText);
	void CreatePakFile(const FString& PakFilePath, const FString& ResponseFilePath);
	void GeneratePakResponseFile(const FString& ResponseFilePath, const FString& FolderToPak);
};

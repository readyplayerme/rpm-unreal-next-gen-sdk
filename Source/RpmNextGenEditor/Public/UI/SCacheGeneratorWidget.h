#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FCacheGenerator;

class SCacheGeneratorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCacheGeneratorWidget) {}
	SLATE_END_ARGS()
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	float ItemsPerCategory = 10.0f;
	FString CacheUrl;
	TSharedPtr<FCacheGenerator> CacheGenerator;
	
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
	
	void OnItemsPerCategoryChanged(float NewValue);


	void OnCacheUrlChanged(const FText& NewText);
};

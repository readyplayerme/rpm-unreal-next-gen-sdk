#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLoader.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class FEditorAssetLoader;
class FGlbLoader;

class SCharacterLoaderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCharacterLoaderWidget) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void OnSkeletonSelected(const FAssetData& AssetData);

private:
	FEditorAssetLoader AssetLoader;
	TSharedPtr<SEditableTextBox> PathTextBox;
	USkeleton* SelectedSkeleton = nullptr;
	
	FText PathText;
	
	FReply OnButtonClick();
	
	void OnPathTextChanged(const FText& NewText);
	void LoadAsset(const FString& Path);
	
	FString GetCurrentSkeletonPath() const;
};

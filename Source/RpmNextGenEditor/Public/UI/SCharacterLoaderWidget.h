#pragma once

#include "CoreMinimal.h"
#include "EditorAssetLoader.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class FEditorAssetLoader;
class FAssetLoader;

class SCharacterLoaderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCharacterLoaderWidget) {}
	SLATE_END_ARGS()

	void OnSkeletonSelected(const FAssetData& AssetData);
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	/** Callback for when the button is clicked */
	FReply OnButtonClick();

	/** Stores the text input by the user */
	FText PathText;

	/** Callback for when the text in the input field changes */
	void OnPathTextChanged(const FText& NewText);

	/** Function that gets called when the button is pressed */
	void LoadAsset(const FString& Path);
	FEditorAssetLoader AssetLoader;
	TSharedPtr<SEditableTextBox> PathTextBox;

	// Store the selected skeleton
	USkeleton* SelectedSkeleton = nullptr;
	FString GetCurrentSkeletonPath() const;
};

#include "UI/SCharacterLoaderWidget.h"

#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "Api/Assets/AssetLoader.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetRegistry/AssetData.h"

// Configuration section and key names
static const FString ConfigSection = TEXT("/Script/ReadyPlayerMe.CharacterLoaderSettings");
static const FString PathKeyName = TEXT("LastFilePath");

void SCharacterLoaderWidget::Construct(const FArguments& InArgs)
{
	FString LastSavedPath;
	GConfig->GetString(*ConfigSection, *PathKeyName, LastSavedPath, GGameIni);
	PathText = FText::FromString(LastSavedPath);
	AssetLoader = FEditorAssetLoader();
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(5)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Enter Path:"))
		]

		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(5)
		[
			SAssignNew(PathTextBox, SEditableTextBox)
			.Text(FText::FromString(LastSavedPath)) // Set the loaded path as the initial text
			.OnTextChanged(this, &SCharacterLoaderWidget::OnPathTextChanged)
		]

		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(5)
		[
			SNew(SButton)
			.Text(FText::FromString("Load Glb"))
			.OnClicked(this, &SCharacterLoaderWidget::OnButtonClick)
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(5)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Select Skeleton:"))
		]

		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(5)
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(USkeleton::StaticClass()) // Only allow USkeleton assets
			.OnObjectChanged(this, &SCharacterLoaderWidget::OnSkeletonSelected) // Handle the asset selection
			.ObjectPath(this, &SCharacterLoaderWidget::GetCurrentSkeletonPath) // Optionally provide a current value
		]
	];
}


void SCharacterLoaderWidget::OnSkeletonSelected(const FAssetData& AssetData)
{
	SelectedSkeleton = Cast<USkeleton>(AssetData.GetAsset());
	if (SelectedSkeleton)
	{
		UE_LOG(LogTemp, Log, TEXT("Selected Skeleton: %s"), *SelectedSkeleton->GetName());
	}
}


FString SCharacterLoaderWidget::GetCurrentSkeletonPath() const
{
	return SelectedSkeleton ? SelectedSkeleton->GetPathName() : FString();
}

void SCharacterLoaderWidget::OnPathTextChanged(const FText& NewText)
{
	PathText = NewText;

	// Save the path to the configuration file
	GConfig->SetString(*ConfigSection, *PathKeyName, *PathText.ToString(), GGameIni);
	GConfig->Flush(false, GGameIni); // Ensure the config is saved immediately
}


FReply SCharacterLoaderWidget::OnButtonClick()
{
	FString Path = PathText.ToString();
	if (Path.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Path is empty"));
		return FReply::Handled();
	}
	LoadAsset(Path);

	return FReply::Handled();
}

void SCharacterLoaderWidget::LoadAsset(const FString& Path)
{
	FglTFRuntimeConfig Config = FglTFRuntimeConfig();
	Config.SceneScale = 100.0f;
	Config.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	UglTFRuntimeAsset* gltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(Path, true, Config);
	if (SelectedSkeleton)
	{
		AssetLoader.SkeletonToCopy = SelectedSkeleton;
	}
	AssetLoader.LoadAssetToWorldAsURpmActor(gltfAsset);
	AssetLoader.SaveAsUAsset(gltfAsset, TEXT("/Game/ReadyPlayerMe/TestSkeleton"));
}

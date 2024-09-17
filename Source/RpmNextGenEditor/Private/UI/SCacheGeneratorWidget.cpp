#include "UI/SCacheGeneratorWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EditorStyleSet.h"
#include "IPlatformFilePak.h"
#include "RpmNextGen.h"
#include "Api/Files/PakFileUtility.h"
#include "Cache/CacheGenerator.h"
#include "Misc/FileHelper.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SScrollBox.h"

void SCacheGeneratorWidget::Construct(const FArguments& InArgs)
{
    if(!CacheGenerator)
    {
        CacheGenerator = MakeUnique<FCacheGenerator>();
        CacheGenerator->OnCacheDataLoaded.BindRaw(this, &SCacheGeneratorWidget::OnFetchCacheDataComplete);
        CacheGenerator->OnDownloadRemoteCacheDelegate.BindRaw(this, &SCacheGeneratorWidget::OnDownloadRemoteCacheComplete);
        CacheGenerator->OnLocalCacheGenerated.BindRaw(this, &SCacheGeneratorWidget::OnGenerateLocalCacheCompleted);
    }
    ChildSlot
    [
        SNew(SScrollBox) // Make the entire content scrollable
        + SScrollBox::Slot()
        .Padding(10)
        [
            SNew(SVerticalBox)

            // Title/Label "Local Cache Generator"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Local Cache Generator"))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
            ]
            // Integer Slider with label "Items per category"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Max items per category:"))
                ]
                + SHorizontalBox::Slot()
                .Padding(5, 0, 0, 0)
                .FillWidth(1.0f)
                [
                    SNew(SNumericEntryBox<int32>)
                    .Value(this, &SCacheGeneratorWidget::GetItemsPerCategory)
                    .OnValueChanged(this, &SCacheGeneratorWidget::OnItemsPerCategoryChanged)
                    .AllowSpin(true) // Slider-like behavior
                    .MinValue(1)
                    .MaxValue(30)
                    .SliderExponent(1.0f)
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Generate offline cache"))
                    .OnClicked(this, &SCacheGeneratorWidget::OnGenerateOfflineCacheClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Extract Cache to local folder"))
                    .OnClicked(this, &SCacheGeneratorWidget::OnExtractCacheClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Open Local Cache Folder"))
                    .OnClicked(this, &SCacheGeneratorWidget::OnOpenLocalCacheFolderClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]
            // TODO implement remote cache download and unzip logic
            // // Title/Label "Remote Cache Downloader"
            // + SVerticalBox::Slot()
            // .Padding(5)
            // .AutoHeight()
            // [
            //     SNew(STextBlock)
            //     .Text(FText::FromString("Remote Cache Downloader"))
            //     .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
            // ]
            //
            // // Editable text field with label "Cache Url"
            // + SVerticalBox::Slot()
            // .Padding(5)
            // .AutoHeight()
            // [
            //     SNew(SHorizontalBox)
            //     + SHorizontalBox::Slot()
            //     .AutoWidth()
            //     .VAlign(VAlign_Center)
            //     [
            //         SNew(STextBlock)
            //         .Text(FText::FromString("Cache URL:"))
            //     ]
            //     + SHorizontalBox::Slot()
            //     .Padding(5, 0, 0, 0)
            //     .FillWidth(1.0f)
            //     [
            //         SNew(SBox)
            //         .HeightOverride(30) // Set text box height
            //         [
            //             SNew(SEditableTextBox)
            //             .Text(FText::FromString("http://"))
            //             .OnTextCommitted(this, &SCacheEditorWidget::OnCacheUrlTextCommitted)
            //         ]
            //     ]
            // ]
            //
            // // Button "Download Remote Cache"
            // + SVerticalBox::Slot()
            // .Padding(5)
            // .AutoHeight()
            // [
            //     SNew(SBox)
            //     .HeightOverride(40) // Set button height
            //     [
            //         SNew(SButton)
            //         .Text(FText::FromString("Download Remote Cache"))
            //         .OnClicked(this, &SCacheEditorWidget::OnDownloadRemoteCacheClicked)
            //         .HAlign(HAlign_Center)
            //         .VAlign(VAlign_Center)
            //     ]
            // ]
        ]
    ];
}


FReply SCacheGeneratorWidget::OnGenerateOfflineCacheClicked()
{
    CacheGenerator->GenerateLocalCache(ItemsPerCategory);
    return FReply::Handled();
}

FReply SCacheGeneratorWidget::OnExtractCacheClicked()
{
    FString PakFilePath = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/RpmAssetCache.pak");
    FPakFileUtility::ExtractPakFile(PakFilePath);
    
    return FReply::Handled();
}

FReply SCacheGeneratorWidget::OnOpenLocalCacheFolderClicked()
{
    const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();
    
    // Check if the folder exists
    if (FPaths::DirectoryExists(GlobalCachePath))
    {
        // Open the folder in the file explorer
        FPlatformProcess::LaunchFileInDefaultExternalApplication(*GlobalCachePath);
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("Folder does not exist: %s"), *GlobalCachePath);
    }

    return FReply::Handled();
}

FReply SCacheGeneratorWidget::OnDownloadRemoteCacheClicked()
{
    // Handle downloading the remote cache
    return FReply::Handled();
}

void SCacheGeneratorWidget::OnFetchCacheDataComplete(bool bWasSuccessful)
{
    UE_LOG(LogReadyPlayerMe, Log, TEXT("Completed fetching assets"));
    CacheGenerator->LoadAndStoreAssets();
}

void SCacheGeneratorWidget::OnDownloadRemoteCacheComplete(bool bWasSuccessful)
{
    
}

void SCacheGeneratorWidget::OnGenerateLocalCacheCompleted(bool bWasSuccessful)
{
    UE_LOG(LogReadyPlayerMe, Log, TEXT("Completed generating cache"));
    UE_LOG(LogReadyPlayerMe, Log, TEXT("Local cache generated successfully"));
    FString FolderToPak = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache");
    FString PakFilePath = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/RpmAssetCache.pak");

    FPakFileUtility::GeneratePakResponseFile(FolderToPak);
    FPakFileUtility::CreatePakFile(PakFilePath);
}

void SCacheGeneratorWidget::OnItemsPerCategoryChanged(float NewValue)
{
    ItemsPerCategory = NewValue;
}

void SCacheGeneratorWidget::OnCacheUrlChanged(const FText& NewText)
{
    CacheUrl = NewText.ToString();
}

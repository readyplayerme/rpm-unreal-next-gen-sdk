#include "UI/SCacheEditorWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EditorStyleSet.h"
#include "IPlatformFilePak.h"
#include "RpmNextGen.h"
#include "Cache/CacheGenerator.h"
#include "Misc/FileHelper.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SScrollBox.h"

void SCacheEditorWidget::Construct(const FArguments& InArgs)
{
    if(!CacheGenerator)
    {
        CacheGenerator = MakeUnique<FCacheGenerator>();
        CacheGenerator->OnCacheDataLoaded.BindRaw(this, &SCacheEditorWidget::OnFetchCacheDataComplete);
        CacheGenerator->OnDownloadRemoteCacheDelegate.BindRaw(this, &SCacheEditorWidget::OnDownloadRemoteCacheComplete);
        CacheGenerator->OnLocalCacheGenerated.BindRaw(this, &SCacheEditorWidget::OnGenerateLocalCacheCompleted);
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
                    .Value(this, &SCacheEditorWidget::GetItemsPerCategory)
                    .OnValueChanged(this, &SCacheEditorWidget::OnItemsPerCategoryChanged)
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
                    .OnClicked(this, &SCacheEditorWidget::OnGenerateOfflineCacheClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]
            // TODO re-enable once we have added unzip logic
            // + SVerticalBox::Slot()
            // .Padding(5)
            // .AutoHeight()
            // [
            //     SNew(SBox)
            //     .HeightOverride(40) // Set button height
            //     [
            //         SNew(SButton)
            //         .Text(FText::FromString("Extract Cache to local folder"))
            //         .OnClicked(this, &SCacheEditorWidget::OnExtractCacheClicked)
            //         .HAlign(HAlign_Center)
            //         .VAlign(VAlign_Center)
            //     ]
            // ]
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Open Local Cache Folder"))
                    .OnClicked(this, &SCacheEditorWidget::OnOpenLocalCacheFolderClicked)
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


FReply SCacheEditorWidget::OnGenerateOfflineCacheClicked()
{
    CacheGenerator->GenerateLocalCache(ItemsPerCategory);
    return FReply::Handled();
}

FReply SCacheEditorWidget::OnExtractCacheClicked()
{
    // Handle extracting the cache
    return FReply::Handled();
}

FReply SCacheEditorWidget::OnOpenLocalCacheFolderClicked()
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
        UE_LOG(LogTemp, Warning, TEXT("Folder does not exist: %s"), *GlobalCachePath);
    }

    return FReply::Handled();
}

FReply SCacheEditorWidget::OnDownloadRemoteCacheClicked()
{
    // Handle downloading the remote cache
    return FReply::Handled();
}

void SCacheEditorWidget::OnFetchCacheDataComplete(bool bWasSuccessful)
{
    UE_LOG(LogReadyPlayerMe, Log, TEXT("Completed fetching assets"));
    CacheGenerator->LoadAndStoreAssets();
}

void SCacheEditorWidget::OnDownloadRemoteCacheComplete(bool bWasSuccessful)
{
}

void SCacheEditorWidget::OnGenerateLocalCacheCompleted(bool bWasSuccessful)
{
    UE_LOG(LogReadyPlayerMe, Log, TEXT("Completed generating cache"));

    //TODO re-nable once zip extraction is implemented
    // UE_LOG(LogReadyPlayerMe, Log, TEXT("Local cache generated successfully"));
    // FString FolderToPak = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/LocalCache");
    // FString PakFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("LocalCacheAssets.pak");
    // FString ResponseFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("RpmCache_ResponseFile.txt");
    //
    // GeneratePakResponseFile(ResponseFilePath, FolderToPak);
    // CreatePakFile(PakFilePath, ResponseFilePath);
}

void SCacheEditorWidget::OnItemsPerCategoryChanged(float NewValue)
{
    ItemsPerCategory = NewValue;
    // Handle slider value change
}

void SCacheEditorWidget::OnCacheUrlChanged(const FText& NewText)
{
    CacheUrl = NewText.ToString();
    // Handle cache URL text change
}

void SCacheEditorWidget::CreatePakFile(const FString& PakFilePath, const FString& ResponseFilePath)
{
    // Path to the UnrealPak executable
    FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));

    // Arguments for the UnrealPak tool
    FString CommandLineArgs = FString::Printf(TEXT("%s -Create=%s"), *PakFilePath, *ResponseFilePath);

    // Launch the UnrealPak process
    FProcHandle ProcHandle = FPlatformProcess::CreateProc(*UnrealPakPath, *CommandLineArgs, true, false, false, nullptr, 0, nullptr, nullptr);

    if (ProcHandle.IsValid())
    {
        FPlatformProcess::WaitForProc(ProcHandle);
        FPlatformProcess::CloseProc(ProcHandle);

        UE_LOG(LogTemp, Log, TEXT("Pak file created successfully: %s"), *PakFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Pak file: %s"), *PakFilePath);
    }
}


void SCacheEditorWidget::GeneratePakResponseFile(const FString& ResponseFilePath, const FString& FolderToPak)
{
    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *FolderToPak, TEXT("*.*"), true, false);

    FString ResponseFileContent;
    int FileCount = 0;
    for (const FString& File : Files)
    {
        FString RelativePath = File;
        FPaths::MakePathRelativeTo(RelativePath, *FolderToPak);
        ResponseFileContent += FString::Printf(TEXT("\"%s\" \"%s\"\n"), *File, *RelativePath);
        FileCount++;
    }
    FFileHelper::SaveStringToFile(ResponseFileContent, *ResponseFilePath);
    // print number of files added to the response file
    UE_LOG(LogTemp, Log, TEXT("Response file created with %d files"), FileCount);
}

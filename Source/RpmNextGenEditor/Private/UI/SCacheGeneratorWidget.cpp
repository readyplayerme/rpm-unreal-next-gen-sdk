#include "UI/SCacheGeneratorWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EditorStyleSet.h"
#include "IPlatformFilePak.h"
#include "RpmNextGen.h"
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
    // Handle extracting the cache
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

    //TODO re-nable once zip extraction is implemented
    // UE_LOG(LogReadyPlayerMe, Log, TEXT("Local cache generated successfully"));
    // FString FolderToPak = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/LocalCache");
    // FString PakFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("LocalCacheAssets.pak");
    // FString ResponseFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("RpmCache_ResponseFile.txt");
    //
    // GeneratePakResponseFile(ResponseFilePath, FolderToPak);
    // CreatePakFile(PakFilePath, ResponseFilePath);
}

void SCacheGeneratorWidget::OnItemsPerCategoryChanged(float NewValue)
{
    ItemsPerCategory = NewValue;
}

void SCacheGeneratorWidget::OnCacheUrlChanged(const FText& NewText)
{
    CacheUrl = NewText.ToString();
}

void SCacheGeneratorWidget::CreatePakFile(const FString& PakFilePath, const FString& ResponseFilePath)
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

        UE_LOG(LogReadyPlayerMe, Log, TEXT("Pak file created successfully: %s"), *PakFilePath);
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to create Pak file: %s"), *PakFilePath);
    }
}


void SCacheGeneratorWidget::GeneratePakResponseFile(const FString& ResponseFilePath, const FString& FolderToPak)
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
    UE_LOG(LogReadyPlayerMe, Log, TEXT("Response file created with %d files"), FileCount);
}

#include "Api/Files/PakFileUtility.h"

#include "Api/Files/FileUtility.h"
#include "IPlatformFilePak.h"

const FString ResponseFilePath = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/Cache/RpmCache_ResponseFile.txt");

#if WITH_EDITOR
const FString FPakFileUtility::CachePakFilePath = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/Cache/RpmAssetCache.pak");
#else
const FString FPakFileUtility::CachePakFilePath = FPaths::ProjectDir() / TEXT("Content/ReadyPlayerMe/Cache/RpmAssetCache.pak");
#endif

void FPakFileUtility::CreatePakFile(const FString& PakFilePath)
{
    const FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
    const FString CommandLineArgs = FString::Printf(TEXT("%s -Create=%s -IoStore"), *PakFilePath, *ResponseFilePath);  // Add the -IoStore flag
    FProcHandle ProcHandle = FPlatformProcess::CreateProc(*UnrealPakPath, *CommandLineArgs, true, false, false, nullptr, 0, nullptr, nullptr);

    if (ProcHandle.IsValid())
    {
        FPlatformProcess::WaitForProc(ProcHandle);
        FPlatformProcess::CloseProc(ProcHandle);

        UE_LOG(LogReadyPlayerMe, Log, TEXT("Pak file and I/O Store files created successfully: %s"), *PakFilePath);
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to create Pak and I/O Store files: %s"), *PakFilePath);
    }
}

void FPakFileUtility::GeneratePakResponseFile()
{
    TArray<FString> Files;
    const FString FolderToPak = FFileUtility::GetCachePath();
    IFileManager::Get().FindFilesRecursive(Files, *FolderToPak, TEXT("*.*"), true, false);

    FString ResponseFileContent;
    int FileCount = 0;
    for (const FString& File : Files)
    {
        FString RelativePath = File;
        FPaths::MakePathRelativeTo(RelativePath, *FolderToPak);  // Make relative to the root of the folder being packed
        

        // Ensure the relative path preserves the subfolder structure
        ResponseFileContent += FString::Printf(TEXT("\"%s\" \"%s\"\n"), *File, *RelativePath);
        FileCount++;
    }

    FFileHelper::SaveStringToFile(ResponseFileContent, *ResponseFilePath);
}

void FPakFileUtility::CreatePakFile()
{    
    GeneratePakResponseFile();
    CreatePakFile(CachePakFilePath);
    IFileManager& FileManager = IFileManager::Get();
    const FString FolderToPak = FFileUtility::GetCachePath();
    // Ensure the folder exists before attempting to delete it
    if (FileManager.DirectoryExists(*FolderToPak))
    {
        // Attempt to delete the directory and all its contents
        bool bDeleted = FileManager.DeleteDirectory(*FolderToPak, false, true);

        if (bDeleted)
        {
            UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully deleted folder: %s"), *FolderToPak);
        }
        else
        {
            UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to delete folder: %s"), *FolderToPak);
        }
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("Folder does not exist: %s"), *FolderToPak);
    }
}

void FPakFileUtility::ExtractPakFile(const FString& PakFilePath)
{
    if(!FPaths::FileExists(PakFilePath) )
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("Pak file does not exist: %s"), *PakFilePath);
        return;
    }
    const FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
    const FString DestinationPath = FFileUtility::GetFullPersistentPath(FFileUtility::RelativeCachePath);
    const FString CommandLineArgs = FString::Printf(TEXT("%s -Extract %s"), *PakFilePath, *DestinationPath);

    FProcHandle ProcHandle = FPlatformProcess::CreateProc(*UnrealPakPath, *CommandLineArgs, true, false, false, nullptr, 0, nullptr, nullptr);

    if (ProcHandle.IsValid())
    {
        FPlatformProcess::WaitForProc(ProcHandle);
        FPlatformProcess::CloseProc(ProcHandle);

        UE_LOG(LogReadyPlayerMe, Log, TEXT("Pak file extracted successfully to: %s"), *DestinationPath);
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to extract Pak file: %s"), *PakFilePath);
    }
}

void FPakFileUtility::ExtractFilesFromPak(const FString& PakFilePath)
{
    if(!FPaths::FileExists(PakFilePath) )
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("Pak file does not exist: %s"), *PakFilePath);
        return;
    }
    
    // Step 1: Get the current platform file and initialize the Pak platform
    IPlatformFile& InnerPlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPakPlatformFile* PakPlatformFile = static_cast<FPakPlatformFile*>(FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")));

    // If the PakPlatformFile is null, initialize it
    if (!PakPlatformFile)
    {
        PakPlatformFile = new FPakPlatformFile();
        FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
        if (!PakPlatformFile->Initialize(&InnerPlatformFile, TEXT("")))
        {
            UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to initialize Pak Platform File"));
            delete PakPlatformFile;
            return;
        }
        UE_LOG(LogReadyPlayerMe, Log, TEXT("Initializing new Pak Platform File"));
    }

    // Step 2: Define mount point and destination paths
    const FString MountPoint = TEXT("/AssetCache/"); // Virtual mount point inside Unreal
    const FString DestinationPath = FFileUtility::GetFullPersistentPath(FFileUtility::RelativeCachePath);

    // Step 3: Mount the Pak file
    if (!PakPlatformFile->Mount(*PakFilePath, 0, *MountPoint))
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to mount Pak file: %s"), *PakFilePath);
        //return;
    }

    UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully mounted Pak file: %s"), *PakFilePath);
    
    // Step 4: List files in the mounted Pak file
    TArray<FString> Files;
    PakPlatformFile->FindFilesRecursively(Files, *MountPoint, TEXT(""));

    if (Files.Num() == 0)
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("No files found in Pak file: %s"), *PakFilePath);
        return;
    }

    UE_LOG(LogReadyPlayerMe, Log, TEXT("Found %d files in Pak file: %s"), Files.Num(), *PakFilePath);

    // Step 5: Extract each file from the Pak
    for (const FString& File : Files)
    {
        // Ensure the file is relative to the mount point
        FString RelativeFilePath = File;
        FPaths::MakePathRelativeTo(RelativeFilePath, *MountPoint);
        FString DestinationFilePath = DestinationPath / RelativeFilePath;

        // Step 6: Handle JSON files separately
        if (File.EndsWith(TEXT(".json")))
        {
            UE_LOG(LogReadyPlayerMe, Log, TEXT("Processing JSON file: %s"), *File);
            FString JsonContent;
            if (FFileHelper::LoadFileToString(JsonContent, *File))
            {
                // Ensure destination directory exists
                IFileManager::Get().MakeDirectory(*FPaths::GetPath(DestinationFilePath), true);

                // Try to deserialize JSON
                TSharedPtr<FJsonObject> JsonObject;
                TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

                if (FJsonSerializer::Deserialize(Reader, JsonObject))
                {
                    UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully parsed JSON content"));
                }
                else
                {
                    UE_LOG(LogReadyPlayerMe, Warning, TEXT("Invalid JSON content in file: %s"), *File);
                }

                // Save JSON file to disk
                if (FFileHelper::SaveStringToFile(JsonContent, *DestinationFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
                {
                    UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully saved JSON file: %s"), *DestinationFilePath);
                }
                else
                {
                    UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to save JSON file: %s"), *DestinationFilePath);
                }
            }
            else
            {
                UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to read JSON file from Pak: %s"), *File);
            }
        }
        else
        {
            // Handle binary files
            TArray<uint8> FileData;
            if (FFileHelper::LoadFileToArray(FileData, *File))
            {
                if (FFileHelper::SaveArrayToFile(FileData, *DestinationFilePath))
                {
                    UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully extracted file: %s"), *DestinationFilePath);
                }
                else
                {
                    UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to save file: %s"), *DestinationFilePath);
                }
            }
            else
            {
                UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to read file from Pak: %s"), *File);
            }
        }
    }

    UE_LOG(LogReadyPlayerMe, Log, TEXT("Finished extracting files from Pak: %s"), *PakFilePath);
}


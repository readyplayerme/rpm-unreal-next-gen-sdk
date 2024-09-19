#include "PakFileUtility.h"
#include "FileUtility.h"
#include "IPlatformFilePak.h"

const FString ResponseFilePath = FPaths::ProjectContentDir() / TEXT("Paks/RpmCache_ResponseFile.txt");

#if WITH_EDITOR
const FString FPakFileUtility::CachePakFilePath = FPaths::ProjectContentDir() / TEXT("Paks/RpmAssetCache.pak");
#else
const FString FPakFileUtility::CachePakFilePath = FPaths::ProjectDir() / TEXT("Content/Paks/RpmAssetCache.pak");
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

        UE_LOG(LogTemp, Log, TEXT("Pak file and I/O Store files created successfully: %s"), *PakFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Pak and I/O Store files: %s"), *PakFilePath);
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
    const FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
    const FString DestinationPath = FFileUtility::GetFullPersistentPath("");
    const FString CommandLineArgs = FString::Printf(TEXT("%s -Extract %s"), *PakFilePath, *DestinationPath);

    FProcHandle ProcHandle = FPlatformProcess::CreateProc(*UnrealPakPath, *CommandLineArgs, true, false, false, nullptr, 0, nullptr, nullptr);

    if (ProcHandle.IsValid())
    {
        FPlatformProcess::WaitForProc(ProcHandle);
        FPlatformProcess::CloseProc(ProcHandle);

        UE_LOG(LogTemp, Log, TEXT("Pak file extracted successfully to: %s"), *DestinationPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to extract Pak file: %s"), *PakFilePath);
    }
}

void FPakFileUtility::ExtractFilesFromPak(const FString& PakFilePath)
{
    IPlatformFile& InnerPlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPakPlatformFile* PakPlatformFile = new FPakPlatformFile();
    FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
    PakPlatformFile->Initialize(&InnerPlatformFile, TEXT(""));
    
    const FString MountPoint = TEXT("/CachePak/");  // Mount in a virtual folder
    const FString DestinationPath = FFileUtility::GetFullPersistentPath(FFileUtility::RelativeCachePath);

    if (PakPlatformFile->Mount(*PakFilePath, 0, *MountPoint))
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully mounted Pak file: %s"), *PakFilePath);

        // Find and extract files from the mounted pak file
        TArray<FString> Files;
        PakPlatformFile->FindFilesRecursively(Files, *MountPoint, TEXT(""));  // Using virtual mount point

        for (const FString& File : Files)
        {
            // Use relative path within the mounted folder, preserving subdirectories
            FString RelativeFilePath = File;
            FPaths::MakePathRelativeTo(RelativeFilePath, *MountPoint);
            FString DestinationFilePath = DestinationPath / RelativeFilePath;

            if (File.EndsWith(TEXT(".json")))
            {
                UE_LOG(LogTemp, Log, TEXT("json File = %s"), *File);
                // Handle JSON files
                FString JsonContent;
                if (FFileHelper::LoadFileToString(JsonContent, *File))
                {
                    UE_LOG(LogTemp, Log, TEXT("Successfully read JSON file: %s"), *File);
                    // Ensure destination folder exists
                    IFileManager::Get().MakeDirectory(*FPaths::GetPath(DestinationFilePath), true);

                    TSharedPtr<FJsonObject> JsonObject;
                    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

                    // Try to deserialize the JSON string into a JsonObject
                    if (FJsonSerializer::Deserialize(Reader, JsonObject))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Json content valid"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Log, TEXT("Json content invalid"));
                    }

                    // Attempt to save the JSON content
                    if (FFileHelper::SaveStringToFile(JsonContent, *DestinationFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Successfully saved JSON file: %s"), *DestinationFilePath);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to save JSON file: %s"), *DestinationFilePath);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to read JSON file from Pak: %s"), *File);
                }
            }
            else
            {
                // Handle other file types as binary
                TArray<uint8> FileData;
                if (FFileHelper::LoadFileToArray(FileData, *File))
                {
                    if (FFileHelper::SaveArrayToFile(FileData, *DestinationFilePath))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Successfully extracted file: %s"), *DestinationFilePath);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to save file: %s"), *DestinationFilePath);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to read file from Pak: %s"), *File);
                }
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Finished extracting files %d from Pak: %s"), Files.Num(), *PakFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to mount Pak file: %s"), *PakFilePath);
    }
}

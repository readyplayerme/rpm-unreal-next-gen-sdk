#include "PakFileUtility.h"
#include "IPlatformFilePak.h"
#include "RpmNextGen.h"

const FString ResponseFilePath = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/RpmCache_ResponseFile.txt");

void FPakFileUtility::CreatePakFile(const FString& PakFilePath)
{
    const FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
    const FString CommandLineArgs = FString::Printf(TEXT("%s -Create=%s"), *PakFilePath, *ResponseFilePath);
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

void FPakFileUtility::GeneratePakResponseFile(const FString& FolderToPak)
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
    UE_LOG(LogTemp, Log, TEXT("Response file created with %d files"), FileCount);
}

void FPakFileUtility::ExtractPakFile(const FString& PakFilePath)
{
    const FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
    const FString DestinationPath = FRpmNextGenModule::GetGlobalAssetCachePath();
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
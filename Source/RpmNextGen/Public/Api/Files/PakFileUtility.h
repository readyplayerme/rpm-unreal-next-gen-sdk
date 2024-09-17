#pragma once

#include "CoreMinimal.h"

class RPMNEXTGEN_API FPakFileUtility
{
public:
	static void CreatePakFile(const FString& PakFilePath);
	static void GeneratePakResponseFile(const FString& FolderToPak);
	static void ExtractPakFile(const FString& PakFilePath);
	static void ExtractFilesFromPak(const FString& PakFilePath);
};
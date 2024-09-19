#pragma once

#include "CoreMinimal.h"

class RPMNEXTGEN_API FPakFileUtility
{
public:
	static void CreatePakFile();
	static void ExtractPakFile(const FString& PakFilePath);
	static void ExtractFilesFromPak(const FString& PakFilePath);
	static const FString CachePakFilePath;
private:
	static void CreatePakFile(const FString& PakFilePath);
	static void GeneratePakResponseFile();
};
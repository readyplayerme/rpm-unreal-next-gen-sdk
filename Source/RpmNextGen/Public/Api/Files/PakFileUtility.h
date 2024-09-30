#pragma once

#include "CoreMinimal.h"

class RPMNEXTGEN_API FPakFileUtility
{
public:
	static const FString CachePakFilePath;
	
	static void CreatePakFile();
	static void ExtractPakFile(const FString& PakFilePath);
	static void ExtractFilesFromPak(const FString& PakFilePath);

private:
	static void CreatePakFile(const FString& PakFilePath);
	static void GeneratePakResponseFile();
};
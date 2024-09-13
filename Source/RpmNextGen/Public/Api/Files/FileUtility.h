#pragma once

#include "CoreMinimal.h"


class RPMNEXTGEN_API FFileUtility
{
	
public:
	static bool SaveToFile(const TArray<uint8>& Data, const FString& FilePath, const bool bSkipIfFileExists = true)
	{
		if ((bSkipIfFileExists && FPaths::FileExists(FilePath)))
		{
			UE_LOG(LogReadyPlayerMe, Log, TEXT("File already exists at: %s. Skipping"), *FilePath);
			return true;
		}
		if (FFileHelper::SaveArrayToFile(Data, *FilePath))
		{
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully saved asset to: %s"), *FilePath);
			return true;
		}

		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to save asset to: %s"), *FilePath);
		return false;
	}
};
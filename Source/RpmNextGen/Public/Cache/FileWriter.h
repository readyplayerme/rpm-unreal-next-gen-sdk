#pragma once

#include "CoreMinimal.h"


class RPMNEXTGEN_API FFileWriter : public TSharedFromThis<FFileWriter>
{
	
public:
	bool SaveToFile(const TArray<uint8>& Data, const FString& FilePath, const bool bSkipIfFileExists = true) const
	{
		if (bSkipIfFileExists && FPaths::FileExists(FilePath) || FFileHelper::SaveArrayToFile(Data, *FilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully saved asset to: %s"), *FilePath);
			return true;
		}
		UE_LOG(LogTemp, Error, TEXT("Failed to save asset to: %s"), *FilePath);
		return false;
	}
};
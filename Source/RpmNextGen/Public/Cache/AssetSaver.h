#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FOnAssetSavedToCache, bool);

class RPMNEXTGEN_API FAssetSaver : public TSharedFromThis<FAssetSaver>
{
	
public:
	void SaveToFile(const FString& FilePath, const TArray<uint8>& Data)
	{
		// skip if file already exists
		if (FPaths::FileExists(FilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Asset already exists at: %s"), *FilePath);
			OnAssetSavedToCache.ExecuteIfBound(true);
			return;
		}
		if (FFileHelper::SaveArrayToFile(Data, *FilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully saved asset to: %s"), *FilePath);
			OnAssetSavedToCache.ExecuteIfBound(true);
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("Failed to save asset to: %s"), *FilePath);
		OnAssetSavedToCache.ExecuteIfBound(false);
	}

	FOnAssetSavedToCache OnAssetSavedToCache;
};
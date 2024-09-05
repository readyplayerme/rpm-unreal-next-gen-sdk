#pragma once
#include "RpmNextGen.h"
#include "AssetSaveData.h"
#include "AssetSaver.h"
#include "Api/Assets/AssetLoader.h"

class FAssetStorageManager
{
public:
	static FAssetStorageManager& Get()
	{
		static FAssetStorageManager Instance;
		return Instance;
	}

	void SaveAssetAndTrack(const FAssetLoadingContext& Context)
	{
		const FAssetSaveData& StoredAsset = FAssetSaveData(Context.Asset, Context.BaseModelId);

		FAssetSaver AssetSaver = FAssetSaver();
		AssetSaver.SaveToFile(StoredAsset.IconFilePath, Context.ImageData);
		AssetSaver.SaveToFile(StoredAsset.GlbFilePath, Context.GlbData);
		TrackStoredAsset(StoredAsset);
	}

	void TrackStoredAsset(const FAssetSaveData& StoredAsset, const bool bSaveManifest = true)
	{
		FAssetSaveData* ExistingStoredAsset = StoredAssets.Find(StoredAsset.Id);
		if(ExistingStoredAsset != nullptr)
		{
			// Update existing stored asset with new values if present
			if(ExistingStoredAsset->GlbFilePath.IsEmpty() && !StoredAsset.GlbFilePath.IsEmpty())
			{
				ExistingStoredAsset->GlbFilePath = StoredAsset.GlbFilePath;
			}
			if(ExistingStoredAsset->IconFilePath.IsEmpty() && !StoredAsset.IconFilePath.IsEmpty())
			{
				ExistingStoredAsset->IconFilePath = StoredAsset.IconFilePath;
			}
		}
		StoredAssets.Add(StoredAsset.Id, ExistingStoredAsset ? *ExistingStoredAsset :  StoredAsset);

		if(bSaveManifest)
		{
			SaveManifest(); 
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Tracked asset: AssetId=%s, GlbFilePath=%s, IconFilePath=%s"), *StoredAsset.Id, *StoredAsset.GlbFilePath, *StoredAsset.IconFilePath);
	}

	void LoadManifest()
	{
		const FString ManifestFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache/AssetManifest.json");
		FString ManifestContent;

		if (FFileHelper::LoadFileToString(ManifestContent, *ManifestFilePath))
		{
			TSharedPtr<FJsonObject> ManifestJson;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestContent);

			if (FJsonSerializer::Deserialize(Reader, ManifestJson) && ManifestJson.IsValid())
			{
				TArray<TSharedPtr<FJsonValue>> AssetsArray = ManifestJson->GetArrayField(TEXT("TrackedAssets"));
				for (const TSharedPtr<FJsonValue>& AssetValue : AssetsArray)
				{
					FAssetSaveData StoredAsset = FAssetSaveData::FromJson(AssetValue->AsObject());
					StoredAssets.Add(StoredAsset.Id, StoredAsset);
				}

				UE_LOG(LogReadyPlayerMe, Log, TEXT("Loaded manifest with %d assets"), StoredAssets.Num());
			}
		}
	}

	void SaveManifest()
	{
		const FString ManifestFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache/AssetManifest.json");

		TSharedPtr<FJsonObject> ManifestJson = MakeShared<FJsonObject>();
		TArray<TSharedPtr<FJsonValue>> AssetsArray;

		for (const auto& Entry : StoredAssets)
		{
			TSharedPtr<FJsonObject> AssetJson = Entry.Value.ToJson();
			AssetsArray.Add(MakeShared<FJsonValueObject>(AssetJson));
		}

		ManifestJson->SetArrayField(TEXT("TrackedAssets"), AssetsArray);

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(ManifestJson.ToSharedRef(), Writer);

		FFileHelper::SaveStringToFile(OutputString, *ManifestFilePath);
	}

	const TMap<FString, FAssetSaveData>& GetStoredAssets() const
	{
		return StoredAssets;
	}

	bool IsAssetCached(const FString& AssetId) const
	{
		return StoredAssets.Contains(AssetId);
	}

	bool GetCachedAsset(const FString& AssetId, FAssetSaveData& OutAsset) const
	{
		const FAssetSaveData* StoredAsset = StoredAssets.Find(AssetId);
		if(StoredAsset != nullptr)
		{
			OutAsset = *StoredAsset;
			return true;
		}
		return false;
	}

private:
	FAssetStorageManager()
	{
		LoadManifest();
	}
	TMap<FString, FAssetSaveData> StoredAssets; 
};

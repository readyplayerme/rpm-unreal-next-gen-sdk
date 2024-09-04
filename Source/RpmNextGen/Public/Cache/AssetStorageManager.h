#pragma once
#include "RpmNextGen.h"
#include "StoredAsset.h"

class FAssetStorageManager
{
public:
	static FAssetStorageManager& Get()
	{
		static FAssetStorageManager Instance;
		return Instance;
	}

	void TrackStoredAsset(const FAsset& Asset, const FString& GlbFilePath, const FString& IconFilePath, const bool bSaveManifest = true)
	{
		FStoredAsset& StoredAsset = StoredAssets.FindOrAdd(Asset.Id);
		StoredAsset.Asset = Asset;
		StoredAsset.GlbFilePath = GlbFilePath;
		StoredAsset.IconFilePath = IconFilePath;

		if(bSaveManifest)
		{
			SaveManifest(); 
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Tracked asset: AssetId=%s, GlbFilePath=%s, IconFilePath=%s"), *Asset.Id, *GlbFilePath, *IconFilePath);
	}

	void TrackStoredAsset(const FStoredAsset& StoredAsset, const bool bSaveManifest = true)
	{
		StoredAssets.Add(StoredAsset.Asset.Id, StoredAsset);

		if(bSaveManifest)
		{
			SaveManifest(); 
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Tracked asset: AssetId=%s, GlbFilePath=%s, IconFilePath=%s"), *StoredAsset.Asset.Id, *StoredAsset.GlbFilePath, *StoredAsset.IconFilePath);
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
					FStoredAsset StoredAsset = FStoredAsset::FromJson(AssetValue->AsObject());
					StoredAssets.Add(StoredAsset.Asset.Id, StoredAsset);
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

	const TMap<FString, FStoredAsset>& GetStoredAssets() const
	{
		return StoredAssets;
	}

private:
	FAssetStorageManager() {}
	TMap<FString, FStoredAsset> StoredAssets; 
};

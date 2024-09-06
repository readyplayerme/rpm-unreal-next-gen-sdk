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

	static void StoreAssetTypes(const TArray<FString>& TypeList)
	{
		FString TypeListFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache/TypeList.json");

		// Convert the TArray<FString> to TArray<TSharedPtr<FJsonValue>>
		TArray<TSharedPtr<FJsonValue>> JsonValues;
		for (const FString& Type : TypeList)
		{
			JsonValues.Add(MakeShared<FJsonValueString>(Type));
		}

		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

		// Serialize the array of TSharedPtr<FJsonValue>
		FJsonSerializer::Serialize(JsonValues, Writer);

		// Save the resulting JSON string to file
		FFileHelper::SaveStringToFile(OutputString, *TypeListFilePath);
	}

	void StoreAndTrackAsset(const FAssetLoadingContext& Context)
	{
		const FAssetSaveData& StoredAsset = FAssetSaveData(Context.Asset, Context.BaseModelId);

		FAssetSaver AssetSaver = FAssetSaver();
		AssetSaver.SaveToFile(StoredAsset.IconFilePath, Context.ImageData);
		AssetSaver.SaveToFile(StoredAsset.GlbFilePath, Context.GlbData);
		StoreAndTrackAsset(StoredAsset);
	}

	void StoreAndTrackAsset(const FAssetSaveData& StoredAsset, const bool bSaveManifest = true)
	{
		FString CombinedId = StoredAsset.Id + StoredAsset.BaseModelId;
		FAssetSaveData* ExistingStoredAsset = StoredAssets.Find(CombinedId);
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
		StoredAssets.Add(CombinedId, ExistingStoredAsset ? *ExistingStoredAsset :  StoredAsset);

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
				const TSharedPtr<FJsonObject> TrackedAssetsJson = ManifestJson->GetObjectField(TEXT("TrackedAssets"));
				for (const auto& Entry : TrackedAssetsJson->Values)
				{
					const FString& AssetId = Entry.Key;
					const TSharedPtr<FJsonObject> AssetData = Entry.Value->AsObject();
					FAssetSaveData StoredAsset = FAssetSaveData::FromJson(AssetData);
					StoredAssets.Add(AssetId, StoredAsset);
				}

				UE_LOG(LogReadyPlayerMe, Log, TEXT("Loaded manifest with %d assets"), StoredAssets.Num());
			}
		}
	}

	void SaveManifest()
	{
		const FString ManifestFilePath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache/AssetManifest.json");

		TSharedPtr<FJsonObject> ManifestJson = MakeShared<FJsonObject>();
		TSharedPtr<FJsonObject> TrackedAssetsJson = MakeShared<FJsonObject>();

		for (const auto& Entry : StoredAssets)
		{
			TSharedPtr<FJsonObject> AssetJson = Entry.Value.ToJson();
			TrackedAssetsJson->SetObjectField(Entry.Key, AssetJson);
		}

		ManifestJson->SetObjectField(TEXT("TrackedAssets"), TrackedAssetsJson);

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

#pragma once
#include "RpmNextGen.h"
#include "CachedAssetData.h"
#include "FileWriter.h"
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
		const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();
		const FString TypeListFilePath = GlobalCachePath / TEXT("TypeList.json");

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
		const FCachedAssetData& StoredAsset = FCachedAssetData(Context.Asset, Context.BaseModelId);

		FFileWriter FileWriter = FFileWriter();
		if(Context.bIsGLb)
		{
			FileWriter.SaveToFile(Context.Data, StoredAsset.GlbPathsByBaseModelId[Context.BaseModelId]);
		}
		else
		{
			UE_LOG(LogReadyPlayerMe, Warning, TEXT("Storing asset Icon in cache at path %s"), *StoredAsset.IconFilePath);
			FileWriter.SaveToFile(Context.Data, StoredAsset.IconFilePath);
		}

		StoreAndTrackAsset(StoredAsset);
	}

	void StoreAndTrackAsset(const FCachedAssetData& StoredAsset, const bool bSaveManifest = true)
	{
		FCachedAssetData* ExistingStoredAsset = StoredAssets.Find(StoredAsset.Id);
		if(ExistingStoredAsset != nullptr)
		{
			// Update existing stored asset with new values if present
			if(!StoredAsset.GlbPathsByBaseModelId.IsEmpty())
			{
				MergeTMaps(ExistingStoredAsset->GlbPathsByBaseModelId, StoredAsset.GlbPathsByBaseModelId);
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
		//UE_LOG(LogReadyPlayerMe, Log, TEXT("Tracked asset: AssetId=%s, GlbFilePath=%s, IconFilePath=%s"), *StoredAsset.Id, *StoredAsset.GlbFilePath, *StoredAsset.IconFilePath);
	}

	void LoadManifest()
	{
		const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();
		const FString ManifestFilePath = GlobalCachePath / TEXT("AssetManifest.json");
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
					FCachedAssetData StoredAsset = FCachedAssetData::FromJson(AssetData);
					StoredAssets.Add(AssetId, StoredAsset);
				}

				UE_LOG(LogReadyPlayerMe, Log, TEXT("Loaded manifest with %d assets"), StoredAssets.Num());
			}
		}
	}

	void SaveManifest()
	{
		const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();
		const FString ManifestFilePath = GlobalCachePath / TEXT("AssetManifest.json");

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

	const TMap<FString, FCachedAssetData>& GetStoredAssets() const
	{
		return StoredAssets;
	}

	bool IsAssetCached(const FString& AssetId) const
	{
		return StoredAssets.Contains(AssetId);
	}

	bool GetCachedAsset(const FString& AssetId, FCachedAssetData& OutAsset) const
	{
		const FCachedAssetData* StoredAsset = StoredAssets.Find(AssetId);
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

	template <typename KeyType, typename ValueType>
	void MergeTMaps(TMap<KeyType, ValueType>& DestinationMap, const TMap<KeyType, ValueType>& SourceMap)
		{
			for (const TPair<KeyType, ValueType>& Elem : SourceMap)
			{
				// Add only if the key doesn't already exist in the destination map
				if (!DestinationMap.Contains(Elem.Key))
				{
					DestinationMap.Add(Elem.Key, Elem.Value);
				}
			}
		}
	TMap<FString, FCachedAssetData> StoredAssets; 
};

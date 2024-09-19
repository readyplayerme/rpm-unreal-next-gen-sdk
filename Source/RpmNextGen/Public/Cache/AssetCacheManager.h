#pragma once
#include "RpmNextGen.h"
#include "CachedAssetData.h"
#include "Api/Assets/AssetLoaderContext.h"
#include "Api/Files/FileUtility.h"

class FAssetCacheManager
{
public:
	static FAssetCacheManager& Get()
	{
		static FAssetCacheManager Instance;
		return Instance;
	}

	static void StoreAssetTypes(const TArray<FString>& TypeList)
	{
		const FString TypeListFilePath = FFileUtility::GetCachePath() / TEXT("TypeList.json");
		
		TArray<TSharedPtr<FJsonValue>> JsonValues;
		for (const FString& Type : TypeList)
		{
			JsonValues.Add(MakeShared<FJsonValueString>(Type));
		}

		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		
		FJsonSerializer::Serialize(JsonValues, Writer);
		
		FFileHelper::SaveStringToFile(OutputString, *TypeListFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	static TArray<FString> LoadAssetTypes()
	{
		const FString GlobalCachePath = FFileUtility::GetCachePath();
		const FString TypeListFilePath = GlobalCachePath / TEXT("TypeList.json");
		FString TypeListContent;

		if (FFileHelper::LoadFileToString(TypeListContent, *TypeListFilePath))
		{
			TArray<TSharedPtr<FJsonValue>> JsonValues;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(TypeListContent);

			if (FJsonSerializer::Deserialize(Reader, JsonValues) && JsonValues.Num() > 0)
			{
				TArray<FString> TypeList;
				for (const TSharedPtr<FJsonValue>& JsonValue : JsonValues)
				{
					TypeList.Add(JsonValue->AsString());
				}
				return TypeList;
			}
		}
		return TArray<FString>();
	}

	TArray<FCachedAssetData> GetAssetsOfType(const FString& AssetType) const
	{
		TArray<FCachedAssetData> Assets;
		for (const auto& Entry : StoredAssets)
		{
			const FCachedAssetData& CachedAsset = Entry.Value;
			if (CachedAsset.Type == AssetType)
			{
				Assets.Add(CachedAsset);
			}
		}
		return Assets;
	}

	void StoreAndTrackIcon(const FAssetLoadingContext& Context, const bool bSaveManifest = true)
	{
		const FCachedAssetData& StoredAsset = FCachedAssetData(Context.Asset);
		FFileUtility::SaveToFile(Context.Data, FFileUtility::GetFullPersistentPath(StoredAsset.IconFilePath));

		StoreAndTrackAsset(StoredAsset, bSaveManifest);
	}
	
	void StoreAndTrackGlb(const FAssetLoadingContext& Context, const bool bSaveManifest = true)
	{
		const FCachedAssetData& StoredAsset = FCachedAssetData(Context.Asset, Context.BaseModelId);
		const FString& GlbPath = FFileUtility::GetFullPersistentPath(StoredAsset.GlbPathsByBaseModelId[Context.BaseModelId]);
		FFileUtility::SaveToFile(Context.Data, GlbPath);
		
		StoreAndTrackAsset(StoredAsset, bSaveManifest);
	}

	void UpdateExistingCachedAsset(const FCachedAssetData& StoredAsset, FCachedAssetData* ExistingStoredAsset)
	{
		if(!StoredAsset.GlbPathsByBaseModelId.IsEmpty())
		{
			MergeTMaps(ExistingStoredAsset->GlbPathsByBaseModelId, StoredAsset.GlbPathsByBaseModelId);
		}
		if(ExistingStoredAsset->IconFilePath.IsEmpty() && !StoredAsset.IconFilePath.IsEmpty())
		{
			ExistingStoredAsset->IconFilePath = StoredAsset.IconFilePath;
		}
	}

	void StoreAndTrackAsset(const FAsset& Asset, const FString& baseModelId = TEXT(""), const bool bSaveManifest = true)
	{
		FCachedAssetData NewCachedAsset = FCachedAssetData(Asset, baseModelId);
		StoreAndTrackAsset(NewCachedAsset, bSaveManifest);
	}

	void StoreAndTrackAsset(const FCachedAssetData& StoredAsset, const bool bSaveManifest = true)
	{
		FCachedAssetData* ExistingStoredAsset = StoredAssets.Find(StoredAsset.Id);
		if(ExistingStoredAsset != nullptr)
		{
			UpdateExistingCachedAsset(StoredAsset, ExistingStoredAsset);
		}
		StoredAssets.Add(StoredAsset.Id, ExistingStoredAsset ? *ExistingStoredAsset :  StoredAsset);

		if(bSaveManifest)
		{
			SaveManifest(); 
		}
	}

	void LoadManifest()
	{
		const FString GlobalCachePath = FFileUtility::GetCachePath();
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
					if(StoredAsset.IsValid())
					{
						StoredAssets.Add(AssetId, StoredAsset);
					}
				}
			}
		}
	}

	void SaveManifest()
	{
		const FString GlobalCachePath = FFileUtility::GetCachePath();
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

		FFileHelper::SaveStringToFile(OutputString, *ManifestFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	void ClearAllCache()
	{
		const FString GlobalCachePath = FFileUtility::GetCachePath();

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (PlatformFile.DirectoryExists(*GlobalCachePath))
		{
			PlatformFile.DeleteDirectoryRecursively(*GlobalCachePath);
			PlatformFile.CreateDirectory(*GlobalCachePath);
		}

		StoredAssets.Empty();
		SaveManifest();
	}

	void RemoveAssetFromCache(const FString& AssetId)
	{
		if (!StoredAssets.Contains(AssetId))
		{
			UE_LOG(LogReadyPlayerMe, Warning, TEXT("No Asset with ID %s found in the cache."), *AssetId);
			return;
		}

		FCachedAssetData CachedAsset = StoredAssets[AssetId];
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		for (const auto& GlbPath : CachedAsset.GlbPathsByBaseModelId)
		{
			if (PlatformFile.FileExists(*GlbPath.Value))
			{
				PlatformFile.DeleteFile(*GlbPath.Value);
			}
		}

		if (PlatformFile.FileExists(*CachedAsset.IconFilePath))
		{
			PlatformFile.DeleteFile(*CachedAsset.IconFilePath);
		}

		StoredAssets.Remove(AssetId);
		SaveManifest();
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset %s of type %s removed from cache"), *AssetId, *CachedAsset.Type);
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
		if(StoredAsset != nullptr && StoredAsset->IsValid())
		{
			OutAsset = *StoredAsset;
			return true;
		}
		return false;
	}

private:
	FAssetCacheManager()
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

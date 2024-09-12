#pragma once
#include "RpmNextGen.h"
#include "CachedAssetData.h"
#include "Api/Assets/AssetLoaderContext.h"
#include "Api/Files/FileWriter.h"

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
		const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();
		const FString TypeListFilePath = GlobalCachePath / TEXT("TypeList.json");
		
		TArray<TSharedPtr<FJsonValue>> JsonValues;
		for (const FString& Type : TypeList)
		{
			JsonValues.Add(MakeShared<FJsonValueString>(Type));
		}

		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		
		FJsonSerializer::Serialize(JsonValues, Writer);
		
		FFileHelper::SaveStringToFile(OutputString, *TypeListFilePath);
	}

	void StoreAndTrackIcon(const FAssetLoadingContext& Context, const bool bSaveManifest = true)
	{
		const FCachedAssetData& StoredAsset = FCachedAssetData(Context.Asset);

		FFileWriter FileWriter = FFileWriter();
		FileWriter.SaveToFile(Context.Data, StoredAsset.IconFilePath);

		StoreAndTrackAsset(StoredAsset, bSaveManifest);
	}
	
	void StoreAndTrackGlb(const FAssetLoadingContext& Context, const bool bSaveManifest = true)
	{
		const FCachedAssetData& StoredAsset = FCachedAssetData(Context.Asset, Context.BaseModelId);

		FFileWriter FileWriter = FFileWriter();

		FileWriter.SaveToFile(Context.Data, StoredAsset.GlbPathsByBaseModelId[Context.BaseModelId]);
		
		StoreAndTrackAsset(StoredAsset, bSaveManifest);
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

	void ClearAllCache()
	{
		const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();

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
		if (StoredAssets.Contains(AssetId))
		{
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
		}
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

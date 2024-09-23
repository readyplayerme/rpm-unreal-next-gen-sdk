#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Files/FileUtility.h"
#include "CachedAssetData.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FCachedAssetData 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "glbUrl"))
	FString GlbUrl;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "iconUrl"))
	FString IconUrl;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TMap<FString, FString> RelativeGlbPathsByBaseModelId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString RelativeIconFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "createdAt"))
	FDateTime CreatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "updatedAt"))
	FDateTime UpdatedAt;

	FCachedAssetData()
	{
		Id = FString();
		Name = FString();
		GlbUrl = FString();
		IconUrl = FString();
		RelativeGlbPathsByBaseModelId = TMap<FString, FString>();
		RelativeIconFilePath = FString();
		Type = FString();
		CreatedAt = FDateTime();
		UpdatedAt = FDateTime();
	}
	FCachedAssetData(const FAsset& InAsset)
	{
		Id = InAsset.Id;
		Name = InAsset.Name;
		GlbUrl = InAsset.GlbUrl;
		IconUrl = InAsset.IconUrl;
		RelativeGlbPathsByBaseModelId = TMap<FString, FString>();
		RelativeIconFilePath = FString::Printf(TEXT("%s/Icons/%s.png"), *FFileUtility::RelativeCachePath, *Id);
		Type = InAsset.Type;
		CreatedAt = InAsset.CreatedAt;
		UpdatedAt = InAsset.UpdatedAt;
	}

	FCachedAssetData(const FAsset& InAsset, const FString& InBaseModelId)
	{
		Id = InAsset.Id;
		Name = InAsset.Name;
		GlbUrl = InAsset.GlbUrl;
		IconUrl = InAsset.IconUrl;
		RelativeGlbPathsByBaseModelId = TMap<FString, FString>();
		if(InBaseModelId != FString())
		{
			RelativeGlbPathsByBaseModelId.Add(InBaseModelId, FString::Printf(TEXT("%s/%s/%s.glb"), *FFileUtility::RelativeCachePath, *InBaseModelId, *Id));
		}
		RelativeIconFilePath = FString::Printf(TEXT("%s/Icons/%s.png"), *FFileUtility::RelativeCachePath, *Id);
		Type = InAsset.Type;
		CreatedAt = InAsset.CreatedAt;
		UpdatedAt = InAsset.UpdatedAt;
	}

	bool IsValid () const
	{
		bool Valid = true;
		if(RelativeGlbPathsByBaseModelId.Num() == 0)
		{
			Valid = false;
		}
		if (RelativeIconFilePath.IsEmpty())
		{
			Valid = false;
		}
		return Valid;
	}

	FAsset ToAsset() const
	{
		FAsset Asset;
		Asset.Id = Id;
		Asset.Name = Name;
		Asset.GlbUrl = GlbUrl;
		Asset.IconUrl = IconUrl;
		Asset.Type = Type;
		Asset.CreatedAt = CreatedAt;
		Asset.UpdatedAt = UpdatedAt;
		return Asset;
	}

	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

		JsonObject->SetStringField(TEXT("Id"), Id);
		JsonObject->SetStringField(TEXT("Name"), Name);
		JsonObject->SetStringField(TEXT("GlbUrl"), GlbUrl);
		JsonObject->SetStringField(TEXT("IconUrl"), IconUrl);
		JsonObject->SetStringField(TEXT("IconFilePath"), RelativeIconFilePath);
		JsonObject->SetStringField(TEXT("Type"), Type);
		JsonObject->SetStringField(TEXT("CreatedAt"), CreatedAt.ToString());
		JsonObject->SetStringField(TEXT("UpdatedAt"), UpdatedAt.ToString());
		
		TSharedPtr<FJsonObject> GlbPathsObject = MakeShared<FJsonObject>();
		for (const auto& Entry : RelativeGlbPathsByBaseModelId)
		{
			GlbPathsObject->SetStringField(Entry.Key, Entry.Value);
		}
		JsonObject->SetObjectField(TEXT("GlbPathsByBaseModelId"), GlbPathsObject);

		return JsonObject;
	}

	static FCachedAssetData FromJson(const TSharedPtr<FJsonObject>& JsonObject)
	{
		FCachedAssetData StoredAsset;

		StoredAsset.Id = JsonObject->GetStringField(TEXT("Id"));
		StoredAsset.Name = JsonObject->GetStringField(TEXT("Name"));
		StoredAsset.GlbUrl = JsonObject->GetStringField(TEXT("GlbUrl"));
		StoredAsset.IconUrl = JsonObject->GetStringField(TEXT("IconUrl"));
		StoredAsset.RelativeIconFilePath = JsonObject->GetStringField(TEXT("IconFilePath"));
		StoredAsset.Type = JsonObject->GetStringField(TEXT("Type"));
		FDateTime::Parse(JsonObject->GetStringField(TEXT("CreatedAt")), StoredAsset.CreatedAt);
		FDateTime::Parse(JsonObject->GetStringField(TEXT("UpdatedAt")), StoredAsset.UpdatedAt);
		
		TSharedPtr<FJsonObject> GlbPathsObject = JsonObject->GetObjectField(TEXT("GlbPathsByBaseModelId"));
		for (const auto& Entry : GlbPathsObject->Values)
		{
			StoredAsset.RelativeGlbPathsByBaseModelId.Add(Entry.Key, Entry.Value->AsString());
		}

		return StoredAsset;
	}

	FString GetGlbPathForBaseModelId(FString BaseModelId)
	{
		if(RelativeGlbPathsByBaseModelId.Num() > 0 && !BaseModelId.IsEmpty())
		{
			return FFileUtility::GetFullPersistentPath(RelativeGlbPathsByBaseModelId[BaseModelId]);
		}
		
		return "";
	}
};

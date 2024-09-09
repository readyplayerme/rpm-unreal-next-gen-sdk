#pragma once

#include "CoreMinimal.h"
#include "RpmNextGen.h"
#include "Api/Assets/Models/Asset.h"
#include "AssetSaveData.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetSaveData 
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
	TMap<FString, FString> GlbPathsByBaseModelId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString IconFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "createdAt"))
	FDateTime CreatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "updatedAt"))
	FDateTime UpdatedAt;

	FAssetSaveData()
	{
		Id = FString();
		Name = FString();
		GlbUrl = FString();
		IconUrl = FString();
		GlbPathsByBaseModelId = TMap<FString, FString>();
		IconFilePath = FString();
		Type = FString();
		CreatedAt = FDateTime();
		UpdatedAt = FDateTime();
	}

	FAssetSaveData(const FAsset& InAsset, const FString& InBaseModelId)
	{
		const FString GlobalCachePath = FRpmNextGenModule::GetGlobalAssetCachePath();
		Id = InAsset.Id;
		Name = InAsset.Name;
		GlbUrl = InAsset.GlbUrl;
		IconUrl = InAsset.IconUrl;
		GlbPathsByBaseModelId = TMap<FString, FString>();
		GlbPathsByBaseModelId.Add(InBaseModelId, FString::Printf(TEXT("%s/%s/%s.glb"), *GlobalCachePath, *InBaseModelId, *Id));
		IconFilePath = FString::Printf(TEXT("%s/Icons/%s.png"), *GlobalCachePath, *Id);
		Type = InAsset.Type;
		CreatedAt = InAsset.CreatedAt;
		UpdatedAt = InAsset.UpdatedAt;
	}

	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

		JsonObject->SetStringField(TEXT("Id"), Id);
		JsonObject->SetStringField(TEXT("Name"), Name);
		JsonObject->SetStringField(TEXT("GlbUrl"), GlbUrl);
		JsonObject->SetStringField(TEXT("IconUrl"), IconUrl);
		JsonObject->SetStringField(TEXT("IconFilePath"), IconFilePath);
		JsonObject->SetStringField(TEXT("Type"), Type);
		JsonObject->SetStringField(TEXT("CreatedAt"), CreatedAt.ToString());
		JsonObject->SetStringField(TEXT("UpdatedAt"), UpdatedAt.ToString());
		
		TSharedPtr<FJsonObject> GlbPathsObject = MakeShared<FJsonObject>();
		for (const auto& Entry : GlbPathsByBaseModelId)
		{
			GlbPathsObject->SetStringField(Entry.Key, Entry.Value);
		}
		JsonObject->SetObjectField(TEXT("GlbPathsByBaseModelId"), GlbPathsObject);

		return JsonObject;
	}

	static FAssetSaveData FromJson(const TSharedPtr<FJsonObject>& JsonObject)
	{
		FAssetSaveData StoredAsset;

		StoredAsset.Id = JsonObject->GetStringField(TEXT("Id"));
		StoredAsset.Name = JsonObject->GetStringField(TEXT("Name"));
		StoredAsset.GlbUrl = JsonObject->GetStringField(TEXT("GlbUrl"));
		StoredAsset.IconUrl = JsonObject->GetStringField(TEXT("IconUrl"));
		StoredAsset.IconFilePath = JsonObject->GetStringField(TEXT("IconFilePath"));
		StoredAsset.Type = JsonObject->GetStringField(TEXT("Type"));
		FDateTime::Parse(JsonObject->GetStringField(TEXT("CreatedAt")), StoredAsset.CreatedAt);
		FDateTime::Parse(JsonObject->GetStringField(TEXT("UpdatedAt")), StoredAsset.UpdatedAt);
		
		TSharedPtr<FJsonObject> GlbPathsObject = JsonObject->GetObjectField(TEXT("GlbPathsByBaseModelId"));
		for (const auto& Entry : GlbPathsObject->Values)
		{
			StoredAsset.GlbPathsByBaseModelId.Add(Entry.Key, Entry.Value->AsString());
		}

		return StoredAsset;
	}
};

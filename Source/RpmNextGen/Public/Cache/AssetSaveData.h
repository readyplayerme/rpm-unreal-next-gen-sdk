#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "AssetSaveData.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FAssetSaveData : public FAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString BaseModelId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString GlbFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString IconFilePath;

	FAssetSaveData()
	{
		BaseModelId = FString();
		IconFilePath = FString();
		GlbFilePath = FString();
	}

	FAssetSaveData(const FAsset& InAsset, const FString& InBaseModelId)
	{
		const FString CacheFolderPath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache");
		Id = InAsset.Id;
		Name = InAsset.Name;
		GlbUrl = InAsset.GlbUrl;
		IconUrl = InAsset.IconUrl;
		Type = InAsset.Type;
		CreatedAt = InAsset.CreatedAt;
		UpdatedAt = InAsset.UpdatedAt;
		BaseModelId = InBaseModelId;
		IconFilePath = FString::Printf(TEXT("%s/%s/%s.png"), *CacheFolderPath, *BaseModelId, *Id);
		GlbFilePath = FString::Printf(TEXT("%s/%s/%s.glb"), *CacheFolderPath, *BaseModelId, *Id);
	}

	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetStringField(TEXT("GlbFilePath"), GlbFilePath);
		JsonObject->SetStringField(TEXT("IconFilePath"), IconFilePath);
		JsonObject->SetStringField(TEXT("BaseModelId"), BaseModelId);
		JsonObject->SetStringField(TEXT("Id"), Id);
		JsonObject->SetStringField(TEXT("Name"), Name);
		JsonObject->SetStringField(TEXT("GlbUrl"), GlbUrl);
		JsonObject->SetStringField(TEXT("IconUrl"), IconUrl);
		JsonObject->SetStringField(TEXT("Type"), Type);
		JsonObject->SetStringField(TEXT("CreatedAt"), CreatedAt.ToString());
		JsonObject->SetStringField(TEXT("UpdatedAt"), UpdatedAt.ToString());

		return JsonObject;
	}

	static FAssetSaveData FromJson(const TSharedPtr<FJsonObject>& JsonObject)
	{
		FAssetSaveData StoredAsset;
		StoredAsset.GlbFilePath = JsonObject->GetStringField(TEXT("GlbFilePath"));
		StoredAsset.IconFilePath = JsonObject->GetStringField(TEXT("IconFilePath"));
		StoredAsset.BaseModelId = JsonObject->GetStringField(TEXT("BaseModelId"));
		StoredAsset.Id = JsonObject->GetStringField(TEXT("Id"));
		StoredAsset.Name = JsonObject->GetStringField(TEXT("Name"));
		StoredAsset.GlbUrl = JsonObject->GetStringField(TEXT("GlbUrl"));
		StoredAsset.IconUrl = JsonObject->GetStringField(TEXT("IconUrl"));
		StoredAsset.Type = JsonObject->GetStringField(TEXT("Type"));
		FDateTime::Parse(JsonObject->GetStringField(TEXT("CreatedAt")), StoredAsset.CreatedAt);
		FDateTime::Parse(JsonObject->GetStringField(TEXT("UpdatedAt")), StoredAsset.UpdatedAt);
		
		return StoredAsset;
	}
};

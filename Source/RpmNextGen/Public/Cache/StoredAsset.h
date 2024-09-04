#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Api/Assets/Models/Asset.h"
#include "StoredAsset.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FStoredAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FAsset Asset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString GlbFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString IconFilePath;

	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetStringField(TEXT("GlbFilePath"), GlbFilePath);
		JsonObject->SetStringField(TEXT("IconFilePath"), IconFilePath);

		// Add the Asset fields as a sub-object
		TSharedPtr<FJsonObject> AssetJson = MakeShared<FJsonObject>();
		FJsonObjectConverter::UStructToJsonObject(FAsset::StaticStruct(), &Asset, AssetJson.ToSharedRef(), 0, 0);
		JsonObject->SetObjectField(TEXT("Asset"), AssetJson);

		return JsonObject;
	}

	static FStoredAsset FromJson(TSharedPtr<FJsonObject> JsonObject)
	{
		FStoredAsset StoredAsset;
		StoredAsset.GlbFilePath = JsonObject->GetStringField(TEXT("GlbFilePath"));
		StoredAsset.IconFilePath = JsonObject->GetStringField(TEXT("IconFilePath"));
		
		const TSharedPtr<FJsonObject> AssetJson = JsonObject->GetObjectField(TEXT("Asset"));
		FJsonObjectConverter::JsonObjectToUStruct(AssetJson.ToSharedRef(), FAsset::StaticStruct(), &StoredAsset.Asset, 0, 0);

		return StoredAsset;
	}
};

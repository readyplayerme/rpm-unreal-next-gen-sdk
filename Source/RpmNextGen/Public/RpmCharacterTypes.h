#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Components/ActorComponent.h"
#include "RpmCharacterTypes.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRpmAnimationConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me" )
	USkeleton* Skeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	TSubclassOf<UAnimInstance> AnimationBlueprint;

	FRpmAnimationConfig()
	{
		Skeleton = nullptr;
		AnimationBlueprint = nullptr;
	}
};

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRpmCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString BaseModelId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "assets"))
	TMap<FString, FAsset> Assets;

	FRpmCharacterData()
	{
		Id = "";
		BaseModelId = "";
		Assets = TMap<FString, FAsset>();
	}
};

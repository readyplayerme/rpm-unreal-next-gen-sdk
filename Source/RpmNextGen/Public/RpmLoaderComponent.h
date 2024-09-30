// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeAsset.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Characters/Models/RpmCharacter.h"
#include "Components/ActorComponent.h"
#include "RpmLoaderComponent.generated.h"

class FFileApi;
class FGlbLoader;
struct FCharacterCreateResponse;
struct FCharacterUpdateResponse;
struct FCharacterFindByIdResponse;
class FCharacterApi;
struct FAsset;

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRpmAnimationConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me" )
	USkeleton* Skeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCreated, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterUpdated, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterFound, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterAssetLoaded, const FRpmCharacterData&, CharacterData, UglTFRuntimeAsset*, GltfRuntimeAsset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewAssetLoaded, const FAsset&, Asset, UglTFRuntimeAsset*, GltfRuntimeAsset );

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPMNEXTGEN_API URpmLoaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URpmLoaderComponent();

	void SetGltfConfig(const FglTFRuntimeConfig* Config);

	void HandleAssetLoaded(TArray<unsigned char>* Data, const FAsset& Asset);

	void HandleCharacterAssetLoaded(TArray<unsigned char>* Array, const FString& FileName);
	
	FglTFRuntimeConfig GltfConfig;
	
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterAssetLoaded OnCharacterAssetLoaded;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnNewAssetLoaded OnNewAssetLoaded;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterCreated OnCharacterCreated;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterUpdated OnCharacterUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterFound OnCharacterFound;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void CreateCharacter(const FString& BaseModelId, bool bUseCache);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadCharacterFromUrl(FString Url);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadGltfRuntimeAssetFromCache(const FAsset& Asset);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadCharacterFromAssetMapCache(TMap<FString, FAsset> AssetMap);
	
	void LoadAssetsFromCacheWithNewStyle();

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadAssetPreview(FAsset AssetData, bool bUseCache);
	
	UFUNCTION()
	virtual void HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful);
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	FString AppId;
	FRpmCharacter Character;
	FRpmCharacterData CharacterData;
	
private:
	TSharedPtr<FCharacterApi> CharacterApi;
	TSharedPtr<FFileApi> FileApi;
};

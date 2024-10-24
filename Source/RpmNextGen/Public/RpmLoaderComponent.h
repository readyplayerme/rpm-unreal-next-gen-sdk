// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeAsset.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Characters/Models/RpmCharacter.h"
#include "Components/ActorComponent.h"
#include "RpmCharacterTypes.h"
#include "RpmLoaderComponent.generated.h"

class FFileApi;
class FGlbLoader;
struct FCharacterCreateResponse;
struct FCharacterUpdateResponse;
struct FCharacterFindByIdResponse;
class FCharacterApi;
struct FAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCreated, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterUpdated, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterFound, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterAssetLoaded, const FRpmCharacterData&, CharacterData, UglTFRuntimeAsset*, GltfRuntimeAsset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssetLoaded, const FAsset&, Asset, UglTFRuntimeAsset*, GltfRuntimeAsset );

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPMNEXTGEN_API URpmLoaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterAssetLoaded OnCharacterAssetLoaded;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnAssetLoaded OnAssetLoaded;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterCreated OnCharacterCreated;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterUpdated OnCharacterUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnCharacterFound OnCharacterFound;

	URpmLoaderComponent();
	
	void SetGltfConfig(const FglTFRuntimeConfig* Config);

	void HandleAssetLoaded(const TArray<uint8>& Data, const FAsset& Asset);
	void HandleCharacterAssetLoaded(const TArray<uint8>& Array, const FString& FileName);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	FglTFRuntimeConfig GltfConfig;
	FString AppId;
	FRpmCharacter Character;
	FRpmCharacterData CharacterData;
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void CreateCharacter(const FString& BaseModelId);

	virtual void UpdateCharacter(const TMap<FString, FString>& Assets);

	virtual void FindCharacterById(const FString CharacterId);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadCharacterFromUrl(FString Url);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadGltfRuntimeAssetFromCache(const FAsset& Asset);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadCharacterAssetsFromCache(TMap<FString, FAsset> AssetMap);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadAssetPreview(FAsset AssetData, bool bUseCache);
	
	virtual void HandleCharacterCreateResponse(TSharedPtr<FCharacterCreateResponse> Response, bool bWasSuccessful);
	virtual void HandleCharacterUpdateResponse(TSharedPtr<FCharacterUpdateResponse> CharacterUpdateResponse, bool bWasSuccessful);
	virtual void HandleCharacterFindResponse(TSharedPtr<FCharacterFindByIdResponse> CharacterFindByIdResponse, bool bWasSuccessful);
	
private:
	TSharedPtr<FCharacterApi> CharacterApi;
	TSharedPtr<FFileApi> FileApi;

	void LoadAssetsFromCacheWithNewStyle();
};

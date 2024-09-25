// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmLoaderComponent.h"

#include "glTFRuntimeFunctionLibrary.h"
#include "RpmFunctionLibrary.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Characters/CharacterApi.h"
#include "Api/Characters/Models/CharacterCreateResponse.h"
#include "Api/Characters/Models/CharacterFindByIdResponse.h"
#include "Api/Characters/Models/CharacterUpdateResponse.h"
#include "Api/Files/GlbLoader.h"
#include "Cache/AssetCacheManager.h"
#include "Settings/RpmDeveloperSettings.h"

URpmLoaderComponent::URpmLoaderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	AppId = RpmSettings->ApplicationId;
	if(GltfConfig == nullptr)
	{
		GltfConfig = new FglTFRuntimeConfig();
	}
	FileApi = MakeShared<FFileApi>();
	FileApi->OnFileRequestComplete.BindUObject(this, &URpmLoaderComponent::LoadGltfRuntimeAsset);
	
	CharacterApi = MakeShared<FCharacterApi>();
	CharacterApi->OnCharacterCreateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterCreateResponse);
	CharacterApi->OnCharacterUpdateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterUpdateResponse);
	CharacterApi->OnCharacterFindResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterFindResponse);
	CharacterData = FRpmCharacterData();
}

void URpmLoaderComponent::SetGltfConfig(FglTFRuntimeConfig* Config)
{
	GltfConfig = Config;
}

void URpmLoaderComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void URpmLoaderComponent::CreateCharacter(const FString& BaseModelId, bool bUseCache)
{
	CharacterData.BaseModelId = BaseModelId;
	if(!FConnectionManager::Get().IsConnected() || bUseCache)
	{
		FCachedAssetData CachedAssetData;
		if(FAssetCacheManager::Get().GetCachedAsset(BaseModelId, CachedAssetData))
		{
			const FAsset AssetFromCache = CachedAssetData.ToAsset();
			CharacterData.Assets.Add(FAssetApi::BaseModelType, AssetFromCache);
			OnCharacterCreated.Broadcast(CharacterData);
			TArray<uint8> Data;
			if(FFileApi::LoadFileFromPath(CachedAssetData.GetGlbPathForBaseModelId(CharacterData.BaseModelId), Data))
			{
				LoadGltfRuntimeAsset(&Data, FAssetApi::BaseModelType);
			}
			return;
		}
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Unable to create character from cache. Will try to create from Url."), *CachedAssetData.Id);
	}

	FCharacterCreateRequest CharacterCreateRequest = FCharacterCreateRequest();
	CharacterCreateRequest.Data.Assets = TMap<FString, FString>();
	CharacterCreateRequest.Data.Assets.Add(FAssetApi::BaseModelType, BaseModelId);
	CharacterCreateRequest.Data.ApplicationId = AppId;
	CharacterApi->CreateAsync(CharacterCreateRequest);
}

void URpmLoaderComponent::LoadCharacterFromUrl(FString Url)
{
	FileApi->LoadFileFromUrl(Url);
}

void URpmLoaderComponent::LoadGltfRuntimeAssetFromCache(const FAsset& Asset)
{
	FCachedAssetData ExistingAsset;
	if(FAssetCacheManager::Get().GetCachedAsset(Asset.Id, ExistingAsset))
	{
		CharacterData.Assets.Add(ExistingAsset.Type, Asset);
		TArray<uint8> Data;
		if(FFileApi::LoadFileFromPath(ExistingAsset.GetGlbPathForBaseModelId(CharacterData.BaseModelId), Data))
		{
			LoadGltfRuntimeAsset( &Data, Asset.Type);
			return;
		}
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset from cache"));
}

void URpmLoaderComponent::LoadCharacterFromAssetMapCache(TMap<FString, FAsset> AssetMap)
{
	TMap<FString, FString> ParamAssets;
	for (auto Pairs : AssetMap)
	{
		ParamAssets.Add(Pairs.Key, Pairs.Value.Id);
	}

	CharacterData.Assets = AssetMap;
	FCharacterPreviewRequest PreviewRequest;
	PreviewRequest.Id = CharacterData.Id;
	PreviewRequest.Params.Assets = ParamAssets;
	const FString& Url = CharacterApi->GeneratePreviewUrl(PreviewRequest);
	LoadCharacterFromUrl(Url);
}

void URpmLoaderComponent::LoadAssetsWithNewStyle()
{
	for (auto PreviewAssets : CharacterData.Assets)
	{
		if(PreviewAssets.Value.Type == FAssetApi::BaseModelType)
		{
			continue;
		}
		 LoadGltfRuntimeAssetFromCache(PreviewAssets.Value);
	}
}

void URpmLoaderComponent::LoadAssetPreview(FAsset AssetData, bool bUseCache)
{
	if (CharacterData.BaseModelId.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("BaseModelId is empty"));
		return;
	}
	const bool bIsBaseModel = AssetData.Type == FAssetApi::BaseModelType;
	if(bIsBaseModel)
	{
		CharacterData.BaseModelId = AssetData.Id;
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Asset is %s setting BaseModelId to AssetId."), *AssetData.Type);
	}
	CharacterData.Assets.Add(AssetData.Type, AssetData);
	
	if(!FConnectionManager::Get().IsConnected() || bUseCache)
	{
		LoadGltfRuntimeAssetFromCache(AssetData);
		if(bIsBaseModel && CharacterData.Assets.Num() > 1)
		{
			LoadAssetsWithNewStyle();
		}
		return;
	}
	TMap<FString, FString> ParamAssets;
	for (auto Asset : CharacterData.Assets)
	{
		ParamAssets.Add(Asset.Key, Asset.Value.Id);
	}
	FCharacterPreviewRequest PreviewRequest;
	PreviewRequest.Id = CharacterData.Id;
	PreviewRequest.Params.Assets = ParamAssets;
	const FString& Url = CharacterApi->GeneratePreviewUrl(PreviewRequest);

	LoadCharacterFromUrl(Url);
}

void URpmLoaderComponent::LoadGltfRuntimeAsset(TArray<unsigned char>* Data, const FString& AssetType)
{
	UglTFRuntimeAsset* GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(*Data, *GltfConfig);
	if(!GltfRuntimeAsset)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset"));
	}
	OnGltfAssetLoaded.Broadcast(GltfRuntimeAsset, AssetType);
}

void URpmLoaderComponent::HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse,
	bool bWasSuccessful)
{
	CharacterData.Id = CharacterCreateResponse.Data.Id;
	OnCharacterCreated.Broadcast(CharacterData);
	LoadCharacterFromUrl(CharacterCreateResponse.Data.GlbUrl);
}

void URpmLoaderComponent::HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse,
	bool bWasSuccessful)
{
	OnCharacterUpdated.Broadcast(CharacterData);
}

void URpmLoaderComponent::HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse,
	bool bWasSuccessful)
{
	OnCharacterFound.Broadcast(CharacterData);
}

void URpmLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


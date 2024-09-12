// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmLoaderComponent.h"

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
	GlbLoader = MakeShared<FGlbLoader>(GltfConfig);
	GlbLoader->OnGLtfAssetLoaded.BindUObject(this, &URpmLoaderComponent::HandleGltfAssetLoaded);
	CharacterApi = MakeShared<FCharacterApi>();
	CharacterApi->OnCharacterCreateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterCreateResponse);
	CharacterApi->OnCharacterUpdateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterUpdateResponse);
	CharacterApi->OnCharacterFindResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterFindResponse);	
}

void URpmLoaderComponent::SetGltfConfig(FglTFRuntimeConfig* Config) const
{
	if(GlbLoader)
	{
		GlbLoader->SetConfig(Config);
	}
}

void URpmLoaderComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void URpmLoaderComponent::CreateCharacter(const FString& BaseModelId, bool bUseCache)
{
	CharacterData.BaseModelId = BaseModelId;
	if(bUseCache)
	{
		FCachedAssetData CachedAssetData;
		if(FAssetCacheManager::Get().GetCachedAsset(BaseModelId, CachedAssetData))
		{
			CharacterData.Assets.Add(FAssetApi::BaseModelType, CachedAssetData.ToAsset());
			OnCharacterCreated.Broadcast(CharacterData);
			GlbLoader->LoadFileFromPath(CachedAssetData.GlbPathsByBaseModelId[BaseModelId], FAssetApi::BaseModelType);
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
	GlbLoader->LoadFileFromUrl(Url);
}

void URpmLoaderComponent::LoadCharacterFromAssetMapCache(TMap<FString, FAsset> AssetMap)
{
	for (auto Pairs : AssetMap)
	{
		
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
	}
	CharacterData.Assets.Add(AssetData.Type, AssetData);
	
	if(bUseCache)
	{
		if(bIsBaseModel)
		{
			CharacterData.Assets.Remove(AssetData.Type);
			for (auto PreviewAssets : CharacterData.Assets)
			{
				FCachedAssetData CachedAsset;
				if(FAssetCacheManager::Get().GetCachedAsset(PreviewAssets.Value.Id, CachedAsset))
				{			
					CharacterData.Assets.Add(CachedAsset.Type, PreviewAssets.Value);
					GlbLoader->LoadFileFromPath(CachedAsset.GlbPathsByBaseModelId[CharacterData.BaseModelId], CachedAsset.Type);
				}
			}
		}
		FCachedAssetData CachedAsset;
		if(FAssetCacheManager::Get().GetCachedAsset(AssetData.Id, CachedAsset))
		{			
			CharacterData.Assets.Add(CachedAsset.Type, AssetData);
			GlbLoader->LoadFileFromPath(CachedAsset.GlbPathsByBaseModelId[CharacterData.BaseModelId], AssetData.Type);
			return;
		}
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Asset %s not found in cache. Will try to fetch from Url."), *AssetData.Id);
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

void URpmLoaderComponent::HandleGltfAssetLoaded(UglTFRuntimeAsset* UglTFRuntimeAsset, const FString& AssetType)
{
	if(!UglTFRuntimeAsset)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset"));
	}
	OnGltfAssetLoaded.Broadcast(UglTFRuntimeAsset, AssetType);
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
	for (auto Element : CharacterUpdateResponse.Data.Assets)
	{
		
	}
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


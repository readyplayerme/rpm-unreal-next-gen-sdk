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
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "Settings/RpmDeveloperSettings.h"

URpmLoaderComponent::URpmLoaderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	AppId = RpmSettings->ApplicationId;
	FileApi = MakeShared<FFileApi>();
	FileApi->OnAssetFileRequestComplete.BindUObject(this, &URpmLoaderComponent::HandleAssetLoaded);
	FileApi->OnFileRequestComplete.BindUObject(this, &URpmLoaderComponent::HandleCharacterAssetLoaded);
	CharacterApi = MakeShared<FCharacterApi>();
	CharacterApi->OnCharacterCreateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterCreateResponse);
	CharacterApi->OnCharacterUpdateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterUpdateResponse);
	CharacterApi->OnCharacterFindResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterFindResponse);
	CharacterData = FRpmCharacterData();
	GltfConfig = FglTFRuntimeConfig();
	GltfConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
}

void URpmLoaderComponent::SetGltfConfig(const FglTFRuntimeConfig* Config)
{
	GltfConfig = *Config;
}

void URpmLoaderComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void URpmLoaderComponent::CreateCharacter(const FString& BaseModelId)
{
	CharacterData.BaseModelId = BaseModelId;
	FAsset BaseModelAsset = FAsset();
	BaseModelAsset.Id = BaseModelId;
	BaseModelAsset.Type = FAssetApi::BaseModelType;
	CharacterData.Assets.Add( FAssetApi::BaseModelType, BaseModelAsset);
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
			UglTFRuntimeAsset* GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Data, GltfConfig);
			if(!GltfRuntimeAsset)
			{
				UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset"));
			}
			OnNewAssetLoaded.Broadcast(Asset, GltfRuntimeAsset);
			return;
		}
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset from cache"));
	OnNewAssetLoaded.Broadcast(Asset, nullptr);
}

void URpmLoaderComponent::LoadCharacterAssetsFromCache(TMap<FString, FAsset> AssetMap)
{
	for (auto Element : AssetMap)
	{
		LoadGltfRuntimeAssetFromCache(Element.Value);
	}
}

void URpmLoaderComponent::LoadAssetsFromCacheWithNewStyle()
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
	}
	CharacterData.Assets.Add(AssetData.Type, AssetData);
	
	if(CharacterData.Id.IsEmpty())
	{
		LoadGltfRuntimeAssetFromCache(AssetData);
		if(bIsBaseModel && CharacterData.Assets.Num() > 1)
		{
			LoadAssetsFromCacheWithNewStyle();
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

	FileApi->LoadAssetFileFromUrl(Url, AssetData);
}

void URpmLoaderComponent::HandleAssetLoaded(const TArray<unsigned char>* Data, const FAsset& Asset)
{
	if(!Data)
	{
		LoadGltfRuntimeAssetFromCache(Asset);
		return;
	}
	UglTFRuntimeAsset* GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(*Data, GltfConfig);
	if(!GltfRuntimeAsset)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset"));
	}
	OnNewAssetLoaded.Broadcast(Asset, GltfRuntimeAsset);
}

void URpmLoaderComponent::HandleCharacterAssetLoaded(const TArray<unsigned char>* Data, const FString& FileName)
{
	if(!Data)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load character asset data"));
		return;
	}
	UglTFRuntimeAsset* GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(*Data, GltfConfig);
	if(!GltfRuntimeAsset)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset"));
	}
	OnCharacterAssetLoaded.Broadcast(CharacterData, GltfRuntimeAsset);
}

void URpmLoaderComponent::HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && CharacterCreateResponse.IsValid())
	{
		CharacterData.Id = CharacterCreateResponse.Data.Id;
		OnCharacterCreated.Broadcast(CharacterData);
		LoadCharacterFromUrl(CharacterCreateResponse.Data.GlbUrl);
		return;
	}
	

	OnCharacterCreated.Broadcast(CharacterData);
	UE_LOG( LogReadyPlayerMe, Error, TEXT("Failed to create character from web Api. Falling back to cache."));
	LoadCharacterAssetsFromCache(CharacterData.Assets);
}

void URpmLoaderComponent::HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful)
{
	OnCharacterUpdated.Broadcast(CharacterData);
}

void URpmLoaderComponent::HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful)
{
	OnCharacterFound.Broadcast(CharacterData);
}

void URpmLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


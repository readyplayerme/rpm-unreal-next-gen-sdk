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
	TSharedPtr<FCharacterCreateRequest> CharacterCreateRequest = MakeShared<FCharacterCreateRequest>();
	CharacterCreateRequest->Data.Assets = TMap<FString, FString>();
	CharacterCreateRequest->Data.Assets.Add(FAssetApi::BaseModelType, BaseModelId);
	CharacterCreateRequest->Data.ApplicationId = AppId;
	CharacterApi->CreateAsync(CharacterCreateRequest, FOnCharacterCreateResponse::CreateUObject(this, &URpmLoaderComponent::HandleCharacterCreateResponse));
}

void URpmLoaderComponent::UpdateCharacter(const TMap<FString, FString>& Assets)
{
	TSharedPtr<FCharacterUpdateRequest> CharacterCreateRequest = MakeShared<FCharacterUpdateRequest>();
	CharacterCreateRequest->Payload.Assets = Assets;
	CharacterApi->UpdateAsync(CharacterCreateRequest , FOnCharacterUpdatResponse::CreateUObject(this, &URpmLoaderComponent::HandleCharacterUpdateResponse));
}

void URpmLoaderComponent::FindCharacterById(const FString CharacterId)
{
	TSharedPtr<FCharacterFindByIdRequest> CharacterFindByIdRequest = MakeShared<FCharacterFindByIdRequest>();
	CharacterFindByIdRequest->Id = CharacterId;
	CharacterApi->FindByIdAsync(CharacterFindByIdRequest, FOnCharacterFindResponse::CreateUObject(this, &URpmLoaderComponent::HandleCharacterFindResponse));
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
			OnAssetLoaded.Broadcast(Asset, GltfRuntimeAsset);
			return;
		}
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset from cache"));
	OnAssetLoaded.Broadcast(Asset, nullptr);
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

	FileApi->LoadFileFromUrl(Url);
}

void URpmLoaderComponent::HandleAssetLoaded(const TArray<uint8>& Data, const FAsset& Asset)
{
	if(Data.Num() == 0)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Invalid or empty data received for asset: %s. Trying to load from cache."), *Asset.Id);
		LoadGltfRuntimeAssetFromCache(Asset);
		return;
	}
	UglTFRuntimeAsset* GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Data, GltfConfig);
	if(!GltfRuntimeAsset)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset."));
	}
	OnAssetLoaded.Broadcast(Asset, GltfRuntimeAsset);
}

void URpmLoaderComponent::HandleCharacterAssetLoaded(const TArray<uint8>& Data, const FString& FileName)
{
	if(Data.Num() == 0)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load character asset data"));
		return;
	}
	UglTFRuntimeAsset* GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Data, GltfConfig);
	if(!GltfRuntimeAsset)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load gltf asset"));
	}
	OnCharacterAssetLoaded.Broadcast(CharacterData, GltfRuntimeAsset);
}

void URpmLoaderComponent::HandleCharacterCreateResponse(TSharedPtr<FCharacterCreateResponse> Response, bool bWasSuccessful)
{
	if(bWasSuccessful && Response.IsValid())
	{
		CharacterData.Id = Response->Data.Id;
		OnCharacterCreated.Broadcast(CharacterData);
		LoadCharacterFromUrl(Response->Data.GlbUrl);
		return;
	}
	

	OnCharacterCreated.Broadcast(CharacterData);
	UE_LOG( LogReadyPlayerMe, Error, TEXT("Failed to create character from web Api. Falling back to cache."));
	LoadCharacterAssetsFromCache(CharacterData.Assets);
}

void URpmLoaderComponent::HandleCharacterUpdateResponse(TSharedPtr<FCharacterUpdateResponse> CharacterUpdateResponse, bool bWasSuccessful)
{
	OnCharacterUpdated.Broadcast(CharacterData);
}

void URpmLoaderComponent::HandleCharacterFindResponse(TSharedPtr<FCharacterFindByIdResponse> CharacterFindByIdResponse, bool bWasSuccessful)
{
	OnCharacterFound.Broadcast(CharacterData);
}

void URpmLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


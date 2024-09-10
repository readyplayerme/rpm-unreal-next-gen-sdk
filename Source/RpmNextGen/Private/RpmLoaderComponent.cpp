// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmLoaderComponent.h"

#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Characters/CharacterApi.h"
#include "Api/Characters/Models/CharacterCreateResponse.h"
#include "Api/Characters/Models/CharacterFindByIdResponse.h"
#include "Api/Characters/Models/CharacterUpdateResponse.h"
#include "Cache/AssetStorageManager.h"
#include "Settings/RpmDeveloperSettings.h"

URpmLoaderComponent::URpmLoaderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	AppId = RpmSettings->ApplicationId;
	CharacterApi = MakeShared<FCharacterApi>();
	PreviewAssetMap = TMap<FString, FString>();
	CharacterApi->OnCharacterCreateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterCreateResponse);
	CharacterApi->OnCharacterUpdateResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterUpdateResponse);
	CharacterApi->OnCharacterFindResponse.BindUObject(this, &URpmLoaderComponent::HandleCharacterFindResponse);	
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
		FAssetSaveData AssetData;
		if(FAssetStorageManager::Get().GetCachedAsset(BaseModelId, AssetData))
		{
			
			CharacterData.Assets.Add(FAssetApi::BaseModelType, AssetData.Id);
			OnCharacterCreated.Broadcast(CharacterData);
		}
	}
	else
	{
		FCharacterCreateRequest CharacterCreateRequest = FCharacterCreateRequest();
		CharacterCreateRequest.Data.Assets = TMap<FString, FString>();
		CharacterCreateRequest.Data.Assets.Add(FAssetApi::BaseModelType, BaseModelId);
		CharacterCreateRequest.Data.ApplicationId = AppId;
		CharacterApi->CreateAsync(CharacterCreateRequest);
	}
}

void URpmLoaderComponent::LoadAssetPreview(FAsset AssetData)
{

	if (Character.Id.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Character Id is empty"));
		return;
	}

	PreviewAssetMap.Add(AssetData.Type, AssetData.Id);
	FCharacterPreviewRequest PreviewRequest;
	PreviewRequest.Id = Character.Id;
	PreviewRequest.Params.Assets = PreviewAssetMap;
	const FString& Url = CharacterApi->GeneratePreviewUrl(PreviewRequest);
	//LoadCharacterFromUrl(Url);
}

void URpmLoaderComponent::HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse,
	bool bWasSuccessful)
{
	CharacterData.Assets.Append(CharacterCreateResponse.Data.Assets);
	CharacterData.Id = CharacterCreateResponse.Data.Id;
	OnCharacterCreated.Broadcast(CharacterData);
}

void URpmLoaderComponent::HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse,
	bool bWasSuccessful)
{
	CharacterData.Assets.Append(CharacterUpdateResponse.Data.Assets);
	OnCharacterUpdated.Broadcast(CharacterData);
}

void URpmLoaderComponent::HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse,
	bool bWasSuccessful)
{
	CharacterData.Assets.Append(CharacterFindByIdResponse.Data.Assets);
	OnCharacterFound.Broadcast(CharacterData);
}


// Called every frame
void URpmLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


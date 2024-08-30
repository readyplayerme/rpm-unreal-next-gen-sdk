// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmPreviewLoaderComponent.h"

#include "RpmNextGen.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Characters/CharacterApi.h"
#include "Api/Characters/Models/CharacterCreateResponse.h"
#include "Api/Characters/Models/CharacterFindByIdResponse.h"
#include "Api/Characters/Models/CharacterUpdateResponse.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmDeveloperSettings;

// Sets default values for this component's properties
URpmPreviewLoaderComponent::URpmPreviewLoaderComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	AppId = RpmSettings->ApplicationId;
	CharacterApi = MakeShared<FCharacterApi>();
	PreviewAssetMap = TMap<FString, FString>();
	CharacterApi->OnCharacterCreateResponse.BindUObject(this, &URpmPreviewLoaderComponent::HandleCharacterCreateResponse);
	CharacterApi->OnCharacterUpdateResponse.BindUObject(this, &URpmPreviewLoaderComponent::HandleCharacterUpdateResponse);
	CharacterApi->OnCharacterFindResponse.BindUObject(this, &URpmPreviewLoaderComponent::HandleCharacterFindResponse);
}

void URpmPreviewLoaderComponent::CreateCharacter()
{
	if(BaseModelId.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("BaseModelId is empty on %s"), *GetOwner()->GetName());
		return;
	}
	FCharacterCreateRequest CharacterCreateRequest = FCharacterCreateRequest();
	CharacterCreateRequest.Data.Assets = TMap<FString, FString>();
	CharacterCreateRequest.Data.Assets.Add("baseModel", BaseModelId);
	CharacterCreateRequest.Data.ApplicationId = AppId;

	CharacterApi->CreateAsync(CharacterCreateRequest);
}

void URpmPreviewLoaderComponent::HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse,
	bool bWasSuccessful)
{
	Character = CharacterCreateResponse.Data;
	LoadCharacter(Character);
}

void URpmPreviewLoaderComponent::HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse,
	bool bWasSuccessful)
{
	Character = CharacterUpdateResponse.Data;
}

void URpmPreviewLoaderComponent::HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse,
	bool bWasSuccessful)
{
	Character = CharacterFindByIdResponse.Data;
}

void URpmPreviewLoaderComponent::LoadCharacter(FRpmCharacter CharacterData)
{
	LoadCharacterFromUrl(CharacterData.GlbUrl);
}

void URpmPreviewLoaderComponent::LoadAssetPreview(FAsset AssetData)
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
	LoadCharacterFromUrl(Url);
}


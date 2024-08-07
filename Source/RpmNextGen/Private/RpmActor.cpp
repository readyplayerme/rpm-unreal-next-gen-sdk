// Fill out your copyright notice in the Description page of Project Settings.

#include "RpmActor.h"
#include "glTFRuntimeAsset.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "HttpModule.h"
#include "Api/Characters/CharacterApi.h"
#include "Api/Characters/Models/RpmCharacter.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/RpmDeveloperSettings.h"

class IHttpRequest;

ARpmActor::ARpmActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RpmSettings = GetMutableDefault<URpmDeveloperSettings>();
	CharacterApi = MakeShared<FCharacterApi>();
	CharacterApi->OnCharacterCreateResponse.BindUObject(this, &ARpmActor::HandleCharacterCreateResponse);
	CharacterApi->OnCharacterUpdateResponse.BindUObject(this, &ARpmActor::HandleCharacterUpdateResponse);
	CharacterApi->OnCharacterFindResponse.BindUObject(this, &ARpmActor::HandleCharacterFindResponse);
    
	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;
	BaseSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BaseSkeletalMesh"));
	BaseSkeletalMeshComponent->SetupAttachment(AssetRoot);
	AddedMeshComponents = TArray<USkeletalMeshComponent*>();
	PreviewAssetMap = TMap<FString, FString>();
	OnSkeletalMeshCallback.BindDynamic(this, &ARpmActor::HandleSkeletalMeshLoaded);
}

void ARpmActor::HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleCharacterCreateResponse"));
	Character = CharacterCreateResponse.Data;
	LoadCharacter(Character);
}

void ARpmActor::HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleCharacterUpdateResponse"));
	Character = CharacterUpdateResponse.Data;
}

void ARpmActor::HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleCharacterFindResponse"));
	Character = CharacterFindByIdResponse.Data;
}

void ARpmActor::CreateCharacter(FString AppId)
{
	//TODO update to fetch first app id
	FCharacterCreateRequest CharacterCreateRequest = FCharacterCreateRequest();
	CharacterCreateRequest.Data.Assets = TMap<FString, FString>();
	CharacterCreateRequest.Data.Assets.Add("baseModel", "663a61055134180105bcdf0d");
	CharacterCreateRequest.Data.ApplicationId = AppId;

	CharacterApi->CreateAsync(CharacterCreateRequest);
}

USkeletalMeshComponent* ARpmActor::CreateSkeletalMeshComponent(USkeletalMesh* SkeletalMesh, const FString& Name)
{
	USkeletalMeshComponent* NewSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, *Name);
	NewSkeletalMeshComponent->SetupAttachment(RootComponent);
	NewSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
	NewSkeletalMeshComponent->SkeletalMesh->SetSkeleton(BaseSkeletalMeshComponent->SkeletalMesh->GetSkeleton());
	NewSkeletalMeshComponent->RegisterComponent();
	AddInstanceComponent(NewSkeletalMeshComponent);
	AddedMeshComponents.Add(NewSkeletalMeshComponent);
	return NewSkeletalMeshComponent;
}

void ARpmActor::LoadglTFAsset(UglTFRuntimeAsset* Asset, const FString& AssetName)
{

	FglTFRuntimeSkeletalMeshConfig* SkeletalMeshConfig = new FglTFRuntimeSkeletalMeshConfig();
	Asset->LoadSkeletalMeshRecursiveAsync("", {}, OnSkeletalMeshCallback, *SkeletalMeshConfig );
}

void ARpmActor::HandleSkeletalMeshLoaded(USkeletalMesh* SkeletalMesh)
{
	//hide starter mesh
	BaseSkeletalMeshComponent->SetVisibility(false);
	CreateSkeletalMeshComponent(SkeletalMesh, "Asset");
}

void ARpmActor::OnAssetDataLoaded(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse,	bool bIsSuccessful)
{
	if (bIsSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Request success from url %s "), *HttpRequest->GetURL());
		FglTFRuntimeConfig Config;
		Config.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
		UglTFRuntimeAsset* gltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(HttpResponse->GetContent(),Config);
		LoadglTFAsset(gltfAsset, "Asset");
	}
	else
	{
		// Handle error
	}
}

void ARpmActor::LoadAsset(FAsset AssetData)
{
	if(Character.Id.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Id is empty"));
		return;
	}
	
	PreviewAssetMap.Add(AssetData.Type, AssetData.Id);
	FCharacterPreviewRequest PreviewRequest;
	PreviewRequest.Id = Character.Id;
	PreviewRequest.Params.Assets = PreviewAssetMap;
	for (auto PreviewAsset : PreviewAssetMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("PreviewAssetMap: %s | Value %s"), *PreviewAsset.Key, *PreviewAsset.Value);
	}
	const FString& Url = CharacterApi->GeneratePreviewUrl(PreviewRequest);
	LoadCharacterUrl(Url);
}

void ARpmActor::LoadCharacterUrl(const FString Url)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	UE_LOG(LogTemp, Warning, TEXT("Requesting from url %s"), *Url);

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ARpmActor::OnAssetDataLoaded);
	HttpRequest->ProcessRequest();
}

void ARpmActor::LoadCharacter(FRpmCharacter CharacterData)
{
	LoadCharacterUrl(CharacterData.GlbUrl);
}

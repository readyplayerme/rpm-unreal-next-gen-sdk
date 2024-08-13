// Fill out your copyright notice in the Description page of Project Settings.

#include "RpmActor.h"
#include "glTFRuntimeAsset.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "HttpModule.h"
#include "Animation/AnimInstance.h"
#include "Api/Characters/CharacterApi.h"
#include "Api/Characters/Models/RpmCharacter.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/RpmDeveloperSettings.h"

class IHttpRequest;

ARpmActor::ARpmActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	URpmDeveloperSettings* RpmSettings = GetMutableDefault<URpmDeveloperSettings>();
	AppId = RpmSettings->ApplicationId;
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
	Character = CharacterCreateResponse.Data;
	LoadCharacter(Character);
}

void ARpmActor::HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful)
{
	Character = CharacterUpdateResponse.Data;
}

void ARpmActor::HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful)
{
	Character = CharacterFindByIdResponse.Data;
}

void ARpmActor::CreateCharacter()
{
	FCharacterCreateRequest CharacterCreateRequest = FCharacterCreateRequest();
	CharacterCreateRequest.Data.Assets = TMap<FString, FString>();
	CharacterCreateRequest.Data.Assets.Add("baseModel", BaseModelId);
	CharacterCreateRequest.Data.ApplicationId = AppId;

	CharacterApi->CreateAsync(CharacterCreateRequest);
}

USkeletalMeshComponent* ARpmActor::CreateSkeletalMeshComponent(USkeletalMesh* SkeletalMesh, const FString& Name)
{
	if(AddedMeshComponents.Num() > 0)
	{
		for (auto AddedMeshComponent : AddedMeshComponents)
		{
			AddedMeshComponent->DestroyComponent();
		}
		AddedMeshComponents.Empty();
	}
	USkeletalMeshComponent* NewSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, *Name);
	NewSkeletalMeshComponent->SetupAttachment(RootComponent);
	NewSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
	NewSkeletalMeshComponent->SkeletalMesh->SetSkeleton(BaseSkeletalMeshComponent->SkeletalMesh->GetSkeleton());
	NewSkeletalMeshComponent->RegisterComponent();
	AddInstanceComponent(NewSkeletalMeshComponent);
	AddedMeshComponents.Add(NewSkeletalMeshComponent);
	
	if (AddedMeshComponents.Num() == 1 && AddedMeshComponents.IsValidIndex(0))
	{
		// Get the SkeletalMeshComponent at index 0
		USkeletalMeshComponent* MeshComponent = AddedMeshComponents[0];

		// Check if the MeshComponent is valid
		if (MeshComponent && TargetAnimBP)
		{
			// Set the animation blueprint class
			MeshComponent->SetAnimInstanceClass(TargetAnimBP);
		}
	}
	else if(AddedMeshComponents.Num() > 1 && AddedMeshComponents.IsValidIndex(0))
	{
		NewSkeletalMeshComponent->SetMasterPoseComponent(AddedMeshComponents[0]);
	}

	return NewSkeletalMeshComponent;
}

void ARpmActor::LoadglTFAsset(UglTFRuntimeAsset* Asset)
{
	Asset->LoadSkeletalMeshRecursiveAsync("", {}, OnSkeletalMeshCallback, SkeletalMeshConfig );
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
		glTFRuntimeConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
		UglTFRuntimeAsset* gltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(HttpResponse->GetContent(), glTFRuntimeConfig);
		LoadglTFAsset(gltfAsset);
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
	const FString& Url = CharacterApi->GeneratePreviewUrl(PreviewRequest);
	LoadCharacterUrl(Url);
}

void ARpmActor::LoadCharacterUrl(const FString Url)
{
	// TODO replace this with use of WebApi class
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
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

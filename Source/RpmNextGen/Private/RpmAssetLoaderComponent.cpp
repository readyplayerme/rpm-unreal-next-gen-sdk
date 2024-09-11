// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmAssetLoaderComponent.h"
#include "RpmNextGen.h"
#include "Api/Files//GlbLoader.h"

class URpmDeveloperSettings;

// Sets default values for this component's properties
URpmAssetLoaderComponent::URpmAssetLoaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	AssetLoader = MakeShared<FGlbLoader>();
	AssetLoader->OnGLtfAssetLoaded.BindUObject(this,&URpmAssetLoaderComponent::HandleGLtfAssetLoaded);
}

// Called when the game starts
void URpmAssetLoaderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URpmAssetLoaderComponent::LoadCharacterFromUrl(const FString Url)
{	
	AssetLoader->LoadFileFromUrl(Url);
}

void URpmAssetLoaderComponent::HandleGLtfAssetLoaded(UglTFRuntimeAsset* gltfAsset, const FString& AssetType)
{
	if (!gltfAsset)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Failed to load gltf asset"));
	}
	OnAssetLoaded.Broadcast(gltfAsset, AssetType);
}

// Called every frame
void URpmAssetLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


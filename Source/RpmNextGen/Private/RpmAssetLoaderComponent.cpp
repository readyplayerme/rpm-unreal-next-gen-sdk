// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmAssetLoaderComponent.h"
#include "Api/Assets/AssetLoader.h"

class URpmDeveloperSettings;

// Sets default values for this component's properties
URpmAssetLoaderComponent::URpmAssetLoaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	AssetLoader = MakeShared<FAssetLoader>();
}

// Called when the game starts
void URpmAssetLoaderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URpmAssetLoaderComponent::LoadCharacterFromUrl(const FString Url)
{	
	AssetLoader->OnGLtfAssetLoaded.BindLambda(
		[this](FString FilePath, UglTFRuntimeAsset* gltfAsset, bool bWasSuccessful)
		{
			if (!gltfAsset || !bWasSuccessful)
			{
				UE_LOG(LogTemp, Log, TEXT("Failed to load gltf asset"));
				return;
			}
			OnGltfAssetLoaded.Broadcast(gltfAsset);
		});

	AssetLoader->LoadGLBFromURL(Url);
}



// Called every frame
void URpmAssetLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


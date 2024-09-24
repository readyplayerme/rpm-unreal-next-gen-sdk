// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "glTFRuntimeSkeletalMeshComponent.h"
#include "RpmNextGen.h"

// Sets default values
ARpmActor::ARpmActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;
}

// Called when the game starts or when spawned
void ARpmActor::BeginPlay()
{
	Super::BeginPlay();
}

void ARpmActor::LoadGltfAssets(TMap<FString, UglTFRuntimeAsset*> GltfAssetsByType)
{
	for (const auto& Pairs : GltfAssetsByType)
	{
		LoadGltfAsset(Pairs.Value, Pairs.Key);
	}
}

void ARpmActor::LoadGltfAsset(UglTFRuntimeAsset* GltfAsset, const FString& AssetType)
{
	if (!GltfAsset)
	{
		UE_LOG(LogGLTFRuntime, Warning, TEXT("No asset to setup"));
		return;
	}

	double LoadingStartTime = FPlatformTime::Seconds();

	RemoveMeshComponentsOfType(AssetType);
	
	const TArray<USceneComponent*> NewMeshComponents = LoadMeshComponents(GltfAsset);
	if (NewMeshComponents.Num() > 0)
	{
		LoadedMeshComponentsByAssetType.Add(AssetType, NewMeshComponents);
	}

	UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset loaded in %f seconds"), FPlatformTime::Seconds() - LoadingStartTime);
}

void ARpmActor::RemoveMeshComponentsOfType(const FString& AssetType)
{
	if (LoadedMeshComponentsByAssetType.Contains(AssetType))
	{
		TArray<USceneComponent*>& ComponentsToRemove = LoadedMeshComponentsByAssetType[AssetType];
		for (USceneComponent* ComponentToRemove : ComponentsToRemove)
		{
			if (ComponentToRemove)
			{
				ComponentToRemove->DestroyComponent();
			}
		}
		LoadedMeshComponentsByAssetType.Remove(AssetType);
	}
}


void ARpmActor::RemoveAllMeshes()
{
	for (const auto Pairs : LoadedMeshComponentsByAssetType){
		
		TArray<USceneComponent*> ComponentsToRemove = Pairs.Value;
		for (USceneComponent* ComponentToRemove : ComponentsToRemove)
		{
			if (ComponentToRemove)
			{
				ComponentToRemove->DestroyComponent();
			}
		}
	}
	LoadedMeshComponentsByAssetType.Empty();
}

TArray<USceneComponent*> ARpmActor::LoadMeshComponents(UglTFRuntimeAsset* GltfAsset)
{
	TArray<FglTFRuntimeNode> AllNodes = GltfAsset->GetNodes();
	TArray<USceneComponent*> NewMeshComponents;
	
	// Loop through all nodes to create mesh components
	for (const FglTFRuntimeNode& Node : AllNodes)
	{
		// Skip bones and armature
		if(GltfAsset->NodeIsBone(Node.Index) || Node.Name.Contains("Armature"))
		{
			continue;
		}
		
		if (Node.SkinIndex >= 0)
		{
			NewMeshComponents.Add(CreateSkeletalMeshComponent(GltfAsset, Node));
		}
		else
		{
			NewMeshComponents.Add(CreateStaticMeshComponent(GltfAsset, Node));

		}
	}
	return NewMeshComponents;
}

USkeletalMeshComponent* ARpmActor::CreateSkeletalMeshComponent(UglTFRuntimeAsset* GltfAsset, const FglTFRuntimeNode& Node)
{
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	if (SkeletalMeshConfig.bPerPolyCollision)
	{
		SkeletalMeshComponent = NewObject<UglTFRuntimeSkeletalMeshComponent>(this, GetSafeNodeName<UglTFRuntimeSkeletalMeshComponent>(Node));
		SkeletalMeshComponent->bEnablePerPolyCollision = true;
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, GetSafeNodeName<USkeletalMeshComponent>(Node));
	}

	USkeletalMesh* SkeletalMesh = GltfAsset->LoadSkeletalMesh(Node.MeshIndex, Node.SkinIndex, SkeletalMeshConfig);
	SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
	SkeletalMeshComponent->SetupAttachment(AssetRoot);
	SkeletalMeshComponent->SetRelativeTransform(Node.Transform);
	SkeletalMeshComponent->RegisterComponent();
	AddInstanceComponent(SkeletalMeshComponent);
	
	return SkeletalMeshComponent;
}

UStaticMeshComponent* ARpmActor::CreateStaticMeshComponent(UglTFRuntimeAsset* GltfAsset, const FglTFRuntimeNode& Node)
{
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	TArray<FTransform> GPUInstancingTransforms;

	if (GltfAsset->GetNodeGPUInstancingTransforms(Node.Index, GPUInstancingTransforms))
	{
		UInstancedStaticMeshComponent* InstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(this, GetSafeNodeName<UInstancedStaticMeshComponent>(Node));
		for (const FTransform& GPUInstanceTransform : GPUInstancingTransforms)
		{
			InstancedStaticMeshComponent->AddInstance(GPUInstanceTransform);
		}
		StaticMeshComponent = InstancedStaticMeshComponent;
	}
	else
	{
		StaticMeshComponent = NewObject<UStaticMeshComponent>(this, GetSafeNodeName<UStaticMeshComponent>(Node));
	}
	
	UStaticMesh* StaticMesh = GltfAsset->LoadStaticMeshLODs({Node.MeshIndex}, StaticMeshConfig);
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetupAttachment(AssetRoot);
	StaticMeshComponent->SetRelativeTransform(Node.Transform);
	StaticMeshComponent->RegisterComponent();
	AddInstanceComponent(StaticMeshComponent);
	
	return StaticMeshComponent;
}

void ARpmActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "glTFRuntimeSkeletalMeshComponent.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"

// Sets default values
ARpmActor::ARpmActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;
	MasterPoseComponent = nullptr;
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
	UE_LOG(LogReadyPlayerMe, Log, TEXT("Start Loading Asset"));

	double LoadingStartTime = FPlatformTime::Seconds();

	RemoveMeshComponentsOfType(AssetType);
	UE_LOG(LogReadyPlayerMe, Log, TEXT("Removed old mesh components"));
	const TArray<USceneComponent*> NewMeshComponents = LoadMeshComponents(GltfAsset, AssetType);
	if (NewMeshComponents.Num() > 0)
	{
		LoadedMeshComponentsByAssetType.Add(AssetType, NewMeshComponents);
		//TODO add check if AnimationCharacter is valid
		// MasterPoseComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		// MasterPoseComponent->SetAnimInstanceClass(AnimationCharacter.AnimationBlueprint);

		UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset loaded in %f seconds"), FPlatformTime::Seconds() - LoadingStartTime);
		return;
	}
	
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load mesh components"));
}

void ARpmActor::LoadGltfAssetWithSkeleton(UglTFRuntimeAsset* GltfAsset, const FString& AssetType, const FRpmAnimationCharacter& InAnimationCharacter)
{
	AnimationCharacter = InAnimationCharacter;
	SkeletalMeshConfig.Skeleton =  AnimationCharacter.Skeleton;
	SkeletalMeshConfig.SkeletonConfig.CopyRotationsFrom =  AnimationCharacter.Skeleton;
	LoadGltfAsset(GltfAsset, AssetType);
}

void ARpmActor::RemoveMeshComponentsOfType(const FString& AssetType)
{
	// Remove all meshes if AssetType is empty as the asset is a full character
	if(AssetType.IsEmpty())
	{
		for (auto Element : LoadedMeshComponentsByAssetType)
		{
			TArray<USceneComponent*>& ComponentsToRemove = Element.Value;
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
	if (!LoadedMeshComponentsByAssetType.IsEmpty() && LoadedMeshComponentsByAssetType.Contains(AssetType))
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

TArray<USceneComponent*> ARpmActor::LoadMeshComponents(UglTFRuntimeAsset* GltfAsset, const FString& AssetType)
{
	TArray<FglTFRuntimeNode> AllNodes = GltfAsset->GetNodes();
	TArray<USceneComponent*> NewMeshComponents;
	//if baseModel or full character asset changes we need to update master pose component
	bool bIsMasterPoseUpdateRequired = AssetType == FAssetApi::BaseModelType || AssetType.IsEmpty();
	
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
			USkeletalMeshComponent* SkeletalMeshComponent = CreateSkeletalMeshComponent(GltfAsset, Node);
			if(AssetType == FAssetApi::BaseModelType && !bIsMasterPoseUpdateRequired)
			{
				MasterPoseComponent = SkeletalMeshComponent;
				NewMeshComponents.Add(SkeletalMeshComponent);
				bIsMasterPoseUpdateRequired = false;
				continue;
			}
			SkeletalMeshComponent->SetMasterPoseComponent(MasterPoseComponent.Get());
			NewMeshComponents.Add(SkeletalMeshComponent);
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

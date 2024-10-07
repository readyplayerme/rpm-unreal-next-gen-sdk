// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "glTFRuntimeSkeletalMeshComponent.h"
#include "RpmNextGen.h"
#include "RpmCharacterTypes.h"
#include "Api/Assets/AssetApi.h"

ARpmActor::ARpmActor()
{
	PrimaryActorTick.bCanEverTick = false;
	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;
	MasterPoseComponent = nullptr;
}

void ARpmActor::BeginPlay()
{
	Super::BeginPlay();
}

void ARpmActor::LoadCharacter(const FRpmCharacterData& InCharacterData, UglTFRuntimeAsset* GltfAsset)
{
	CharacterData = InCharacterData;
	if(AnimationConfigsByBaseModelId.Contains(CharacterData.BaseModelId))
	{
		AnimationConfig = AnimationConfigsByBaseModelId[CharacterData.BaseModelId];
		SkeletalMeshConfig.Skeleton =  AnimationConfig.Skeleton;
		SkeletalMeshConfig.SkeletonConfig.CopyRotationsFrom =  AnimationConfig.Skeleton;
	}
	LoadAsset(FAsset(), GltfAsset);
}

void ARpmActor::LoadAsset(const FAsset& Asset, UglTFRuntimeAsset* GltfAsset)
{
	if (!GltfAsset)
	{
		UE_LOG(LogGLTFRuntime, Warning, TEXT("No asset to setup"));
		return;
	}
	if(Asset.Type == FAssetApi::BaseModelType)
	{
		CharacterData.BaseModelId = Asset.Id;
		if(AnimationConfigsByBaseModelId.Contains(CharacterData.BaseModelId))
		{
			AnimationConfig = AnimationConfigsByBaseModelId[CharacterData.BaseModelId];
			SkeletalMeshConfig.Skeleton =  AnimationConfig.Skeleton;
			SkeletalMeshConfig.SkeletonConfig.CopyRotationsFrom =  AnimationConfig.Skeleton;
		}
	}
	RemoveMeshComponentsOfType(Asset.Type);
	double LoadingStartTime = FPlatformTime::Seconds();

	const TArray<USceneComponent*> NewMeshComponents = LoadMeshComponents(GltfAsset, Asset.Type);
	if (NewMeshComponents.Num() > 0)
	{
		LoadedMeshComponentsByAssetType.Add(Asset.Type, NewMeshComponents);
		if(AnimationConfigsByBaseModelId.Contains(CharacterData.BaseModelId))
		{
			if (MasterPoseComponent == nullptr)
			{
				UE_LOG(LogReadyPlayerMe, Error, TEXT("MasterPoseComponent is null for base model %s"), *CharacterData.BaseModelId);
				return;
			}

			if (!AnimationConfig.AnimationBlueprint)
			{
				UE_LOG(LogReadyPlayerMe, Error, TEXT("AnimationBlueprint is null for base model %s"), *CharacterData.BaseModelId);
				return;
			}
			
			MasterPoseComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			MasterPoseComponent->SetAnimClass(AnimationConfig.AnimationBlueprint);
		}

		UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset loaded in %f seconds"), FPlatformTime::Seconds() - LoadingStartTime);
		return;
	}
	
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load mesh components"));
}

void ARpmActor::LoadGltfAssetWithSkeleton(UglTFRuntimeAsset* GltfAsset, const FAsset& Asset, const FRpmAnimationConfig& InAnimationCharacter)
{
	AnimationConfig = InAnimationCharacter;
	SkeletalMeshConfig.Skeleton =  AnimationConfig.Skeleton;
	SkeletalMeshConfig.SkeletonConfig.CopyRotationsFrom =  AnimationConfig.Skeleton;
	LoadAsset(Asset, GltfAsset);
}

void ARpmActor::RemoveMeshComponentsOfType(const FString& AssetType)
{
	if (LoadedMeshComponentsByAssetType.IsEmpty())
	{
		UE_LOG( LogReadyPlayerMe, Log, TEXT("No mesh components to remove"));
		return;
	}

	// Remove components by type, or remove all if AssetType is empty or it's a new base model
	if (AssetType.IsEmpty() || AssetType == FAssetApi::BaseModelType)
	{
		RemoveAllMeshes();
	}
	else if (TArray<USceneComponent*>* ComponentsToRemove = LoadedMeshComponentsByAssetType.Find(AssetType))
	{
		RemoveMeshComponents(*ComponentsToRemove);
		LoadedMeshComponentsByAssetType.Remove(AssetType);
	}
}

void ARpmActor::RemoveAllMeshes()
{
	for (const auto& Pair : LoadedMeshComponentsByAssetType)
	{
		RemoveMeshComponents(Pair.Value);
	}
	LoadedMeshComponentsByAssetType.Empty();
}

void ARpmActor::RemoveMeshComponents(const TArray<USceneComponent*>& Components)
{
	for (USceneComponent* Component : Components)
	{
		if (Component)
		{
			Component->DestroyComponent();
		}
	}
}

TArray<USceneComponent*> ARpmActor::LoadMeshComponents(UglTFRuntimeAsset* GltfAsset, const FString& AssetType)
{
	TArray<FglTFRuntimeNode> AllNodes = GltfAsset->GetNodes();
	TArray<USceneComponent*> NewMeshComponents;
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
			if(bIsMasterPoseUpdateRequired)
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
	if (!SkeletalMesh)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load skeletal mesh for node %s"), *Node.Name);
		return nullptr;
	}
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

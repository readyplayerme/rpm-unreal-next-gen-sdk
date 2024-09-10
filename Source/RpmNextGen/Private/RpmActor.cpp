// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "glTFRuntimeSkeletalMeshComponent.h"
#include "RpmNextGen.h"

// Sets default values
ARpmActor::ARpmActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;
	bStaticMeshesAsSkeletalOnMorphTargets = true;
}

// Called when the game starts or when spawned
void ARpmActor::BeginPlay()
{
	Super::BeginPlay();

	if (!Asset)
	{
		return;
	}
	
	SetupAsset();
}

void ARpmActor::LoadGltfAsset(UglTFRuntimeAsset* GltfAsset)
{
	// Before loading a new asset, clear existing components
	ClearLoadedComponents();
	Asset = GltfAsset;
	SetupAsset();
}

void ARpmActor::ClearLoadedComponents()
{
	if (RootComponent)
	{
		TArray<USceneComponent*> ChildComponents;
		RootComponent->GetChildrenComponents(true, ChildComponents);

		for (USceneComponent* ChildComponent : ChildComponents)
		{
			if (ChildComponent && ChildComponent != RootComponent)
			{
				ChildComponent->DestroyComponent();
			}
		}
	}
}

void ARpmActor::SetupAsset()
{
	if (!Asset)
	{
		UE_LOG(LogGLTFRuntime, Warning, TEXT("No asset to setup"));
		return;
	}

	double LoadingStartTime = FPlatformTime::Seconds();


	TArray<FglTFRuntimeScene> Scenes = Asset->GetScenes();
	for (FglTFRuntimeScene& Scene : Scenes)
	{
		for (int32 NodeIndex : Scene.RootNodesIndices)
		{
			FglTFRuntimeNode Node;
			if (!Asset->GetNode(NodeIndex, Node))
			{
				return;
			}
			ProcessNode(AssetRoot, NAME_None, Node);
		}
	}

	for (TPair<USceneComponent*, FName>& Pair : SocketMapping)
	{
		for (USkeletalMeshComponent* SkeletalMeshComponent : DiscoveredSkeletalMeshComponents)
		{
			if (SkeletalMeshComponent->DoesSocketExist(Pair.Value))
			{
				Pair.Key->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, Pair.Value);
				Pair.Key->SetRelativeTransform(FTransform::Identity);
				break;
			}
		}
	}

	UE_LOG(LogGLTFRuntime, Log, TEXT("Asset loaded in %f seconds"), FPlatformTime::Seconds() - LoadingStartTime);
}

void ARpmActor::LoadClothingAsset(UglTFRuntimeAsset* GltfAsset, const FString& ClothingType)
{
	// Remove existing components of this clothing type, if any
	RemoveClothingComponents(ClothingType);

	// Create and add the new components for the clothing type (multiple meshes)
	TArray<USceneComponent*> NewClothingComponents = CreateClothingMeshComponents(GltfAsset, ClothingType);
	if (NewClothingComponents.Num() > 0)
	{
		LoadedClothingComponents.Add(ClothingType, NewClothingComponents);
	}
}

void ARpmActor::RemoveClothingComponents(const FString& ClothingType)
{
	if (LoadedClothingComponents.Contains(ClothingType))
	{
		TArray<USceneComponent*>& OldComponents = LoadedClothingComponents[ClothingType];
		for (USceneComponent* OldComponent : OldComponents)
		{
			if (OldComponent)
			{
				OldComponent->DestroyComponent();
			}
		}
		LoadedClothingComponents.Remove(ClothingType);
	}
}

TArray<USceneComponent*> ARpmActor::CreateClothingMeshComponents(UglTFRuntimeAsset* GltfAsset, const FString& ClothingType)
{
	TArray<USceneComponent*> NewComponents;

	// Assuming the asset contains multiple meshes, loop through and load them
	// TArray<int32> MeshIndices = GltfAsset->GetMeshIndices();  // Or some other method to get mesh indices
	//
	// for (int32 MeshIndex : MeshIndices)
	// {
	// 	USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, *FString::Printf(TEXT("%s_Mesh_%d"), *ClothingType, MeshIndex));
	// 	USkeletalMesh* SkeletalMesh = GltfAsset->LoadSkeletalMesh(MeshIndex);
	//
	// 	if (SkeletalMesh)
	// 	{
	// 		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
	// 		SkeletalMeshComponent->SetupAttachment(RootComponent);
	// 		SkeletalMeshComponent->RegisterComponent();
	// 		SkeletalMeshComponent->SetRelativeTransform(FTransform::Identity);
	//
	// 		NewComponents.Add(SkeletalMeshComponent);
	//
	// 		// Custom event handling for when a skeletal mesh component is created
	// 		ReceiveOnSkeletalMeshComponentCreated(SkeletalMeshComponent, MeshIndex);
	// 	}
	// }

	return NewComponents;
}


void ARpmActor::ProcessNode(USceneComponent* NodeParentComponent, const FName SocketName, FglTFRuntimeNode& Node)
{
	// Skip the "Armature" node but still process its children
	if (Node.Name.Contains("Armature"))
	{
		UE_LOG(LogGLTFRuntime, Log, TEXT("Skipping Armature node but processing its children"));

		// Process children of the "Armature" node
		ProcessChildNodes(NodeParentComponent, Node);
		return;
	}
	
	if (Asset->NodeIsBone(Node.Index))
	{
		ProcessBoneNode(NodeParentComponent, Node);
		return;
	}

	USceneComponent* NewComponent = CreateNewComponent(NodeParentComponent, Node);

	if (!NewComponent)
	{
		return;
	}

	SetupComponentTags(NewComponent, Node, SocketName);
	ProcessChildNodes(NewComponent, Node);
}

void ARpmActor::ProcessBoneNode(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node)
{
	for (int32 ChildIndex : Node.ChildrenIndices)
	{
		FglTFRuntimeNode Child;
		if (!Asset->GetNode(ChildIndex, Child))
		{
			return;
		}
		ProcessNode(NodeParentComponent, *Child.Name, Child);
	}
}

USceneComponent* ARpmActor::CreateNewComponent(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node)
{
	USceneComponent* NewComponent = nullptr;

	if (Node.SkinIndex >= 0 || (bStaticMeshesAsSkeletalOnMorphTargets && Asset->MeshHasMorphTargets(Node.MeshIndex)))
	{
		NewComponent = CreateSkeletalMeshComponent(NodeParentComponent, Node);
		if(Node.Name.Contains("Armature"))
		{
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Armature found when creating Skeletal mesh"));
		}
	}
	else
	{
		NewComponent = CreateStaticMeshComponent(NodeParentComponent, Node);
		
	}
	
	AddInstanceComponent(NewComponent);

	return NewComponent;
}


USkeletalMeshComponent* ARpmActor::CreateSkeletalMeshComponent(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node)
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

	// Load and set the skeletal mesh
	USkeletalMesh* SkeletalMesh = Asset->LoadSkeletalMesh(Node.MeshIndex, Node.SkinIndex, SkeletalMeshConfig);
	SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);

	// Attach to AssetRoot and register the component
	SkeletalMeshComponent->SetupAttachment(AssetRoot);
	SkeletalMeshComponent->RegisterComponent();
	SkeletalMeshComponent->SetRelativeTransform(Node.Transform);

	// Add the component to the list of discovered skeletal mesh components
	DiscoveredSkeletalMeshComponents.Add(SkeletalMeshComponent);

	// Custom event handling for when a skeletal mesh component is created
	ReceiveOnSkeletalMeshComponentCreated(SkeletalMeshComponent, Node);

	return SkeletalMeshComponent;
}

UStaticMeshComponent* ARpmActor::CreateStaticMeshComponent(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node)
{
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	TArray<FTransform> GPUInstancingTransforms;

	if (Asset->GetNodeGPUInstancingTransforms(Node.Index, GPUInstancingTransforms))
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

	// Load and set the static mesh
	UStaticMesh* StaticMesh = Asset->LoadStaticMeshLODs({Node.MeshIndex}, StaticMeshConfig);
	StaticMeshComponent->SetStaticMesh(StaticMesh);

	// Attach to AssetRoot and register the component
	StaticMeshComponent->SetupAttachment(AssetRoot);
	StaticMeshComponent->RegisterComponent();
	StaticMeshComponent->SetRelativeTransform(Node.Transform);

	// Custom event handling for when a static mesh component is created
	ReceiveOnStaticMeshComponentCreated(StaticMeshComponent, Node);

	return StaticMeshComponent;
}


void ARpmActor::SetupComponentTags(USceneComponent* Component, FglTFRuntimeNode& Node, const FName SocketName)
{
	Component->ComponentTags.Add(*FString::Printf(TEXT("glTFRuntime:NodeName:%s"), *Node.Name));
	Component->ComponentTags.Add(*FString::Printf(TEXT("glTFRuntime:NodeIndex:%d"), Node.Index));
    
	if (SocketName != NAME_None)
	{
		SocketMapping.Add(Component, SocketName);
	}
}

void ARpmActor::ProcessChildNodes(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node)
{
	for (int32 ChildIndex : Node.ChildrenIndices)
	{
		FglTFRuntimeNode Child;
		if (!Asset->GetNode(ChildIndex, Child))
		{
			return;
		}
		ProcessNode(NodeParentComponent, NAME_None, Child);
	}
}

// Called every frame
void ARpmActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARpmActor::ReceiveOnStaticMeshComponentCreated_Implementation(UStaticMeshComponent* StaticMeshComponent, const FglTFRuntimeNode& Node)
{

}

void ARpmActor::ReceiveOnSkeletalMeshComponentCreated_Implementation(USkeletalMeshComponent* SkeletalMeshComponent, const FglTFRuntimeNode& Node)
{
	
}

void ARpmActor::PostUnregisterAllComponents()
{
	if (Asset)
	{
		Asset->ClearCache();
		Asset = nullptr;
	}
	Super::PostUnregisterAllComponents();
}

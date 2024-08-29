// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "glTFRuntimeSkeletalMeshComponent.h"

// Sets default values
ARpmActor::ARpmActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;
	RootNodeIndex = INDEX_NONE;
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

	if (RootNodeIndex > INDEX_NONE)
	{
		FglTFRuntimeNode Node;
		if (!Asset->GetNode(RootNodeIndex, Node))
		{
			return;
		}
		AssetRoot = nullptr;
		ProcessNode(nullptr, NAME_None, Node);
	}
	else
	{
		TArray<FglTFRuntimeScene> Scenes = Asset->GetScenes();
		for (FglTFRuntimeScene& Scene : Scenes)
		{
			USceneComponent* SceneComponent = NewObject<USceneComponent>(this, *FString::Printf(TEXT("Scene %d"), Scene.Index));
			SceneComponent->SetupAttachment(RootComponent);
			SceneComponent->RegisterComponent();
			AddInstanceComponent(SceneComponent);
			for (int32 NodeIndex : Scene.RootNodesIndices)
			{
				FglTFRuntimeNode Node;
				if (!Asset->GetNode(NodeIndex, Node))
				{
					return;
				}
				ProcessNode(SceneComponent, NAME_None, Node);
			}
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


void ARpmActor::ProcessNode(USceneComponent* NodeParentComponent, const FName SocketName, FglTFRuntimeNode& Node)
{
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

    // Check if the node should be a skeletal mesh component
    if (Node.SkinIndex >= 0 || (bStaticMeshesAsSkeletalOnMorphTargets && Asset->MeshHasMorphTargets(Node.MeshIndex)))
    {
        // Create a skeletal mesh component
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

        // Attach and register the component
        SkeletalMeshComponent->SetupAttachment(NodeParentComponent ? NodeParentComponent : RootComponent.Get());
        SkeletalMeshComponent->RegisterComponent();
        SkeletalMeshComponent->SetRelativeTransform(Node.Transform);

        // Add the component to the list of discovered skeletal mesh components
        DiscoveredSkeletalMeshComponents.Add(SkeletalMeshComponent);

        NewComponent = SkeletalMeshComponent;

        // Custom event handling for when a skeletal mesh component is created
        ReceiveOnSkeletalMeshComponentCreated(SkeletalMeshComponent, Node);
    }
    else
    {
        // Create a static mesh component
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

        // Attach and register the component
        StaticMeshComponent->SetupAttachment(NodeParentComponent ? NodeParentComponent : RootComponent.Get());
        StaticMeshComponent->RegisterComponent();
        StaticMeshComponent->SetRelativeTransform(Node.Transform);

        NewComponent = StaticMeshComponent;

        // Custom event handling for when a static mesh component is created
        ReceiveOnStaticMeshComponentCreated(StaticMeshComponent, Node);
    }

    // Add the component to the actor's list of instance components
    AddInstanceComponent(NewComponent);

    return NewComponent;
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

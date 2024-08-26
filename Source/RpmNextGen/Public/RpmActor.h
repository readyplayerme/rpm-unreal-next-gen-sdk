// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "glTFRuntimeAsset.h"
#include "RpmActor.generated.h"

UCLASS()
class RPMNEXTGEN_API ARpmActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARpmActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void ProcessNode(USceneComponent* NodeParentComponent, const FName SocketName, FglTFRuntimeNode& Node);

	template<typename T>
	FName GetSafeNodeName(const FglTFRuntimeNode& Node)
	{
		return MakeUniqueObjectName(this, T::StaticClass(), *Node.Name);
	}

	UPROPERTY()
	TMap<USceneComponent*, FName> SocketMapping;
	UPROPERTY()
	TArray<USkeletalMeshComponent*> DiscoveredSkeletalMeshComponents;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|glTFRuntime")
	UglTFRuntimeAsset* Asset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|glTFRuntime")
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|glTFRuntime")
	FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|glTFRuntime")
	FglTFRuntimeSkeletalAnimationConfig SkeletalAnimationConfig;

	UFUNCTION(BlueprintNativeEvent, Category = "Ready Player Me|glTFRuntime", meta = (DisplayName = "On StaticMeshComponent Created"))
	void ReceiveOnStaticMeshComponentCreated(UStaticMeshComponent* StaticMeshComponent, const FglTFRuntimeNode& Node);

	UFUNCTION(BlueprintNativeEvent, Category = "Ready Player Me|glTFRuntime", meta = (DisplayName = "On SkeletalMeshComponent Created"))
	void ReceiveOnSkeletalMeshComponentCreated(USkeletalMeshComponent* SkeletalMeshComponent, const FglTFRuntimeNode& Node);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|glTFRuntime")
	int32 RootNodeIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|glTFRuntime")
	bool bStaticMeshesAsSkeletalOnMorphTargets;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FglTFRuntimeAssetActorNodeProcessed, const FglTFRuntimeNode&, USceneComponent*);
	FglTFRuntimeAssetActorNodeProcessed OnNodeProcessed;

	virtual void PostUnregisterAllComponents() override;

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadGltfAsset(UglTFRuntimeAsset* GltfAsset);

	virtual void SetupAsset();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Ready Player Me|glTFRuntime")
	USceneComponent* AssetRoot;

	void ProcessBoneNode(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node);
	USceneComponent* CreateNewComponent(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node);
	void SetupComponentTags(USceneComponent* Component, FglTFRuntimeNode& Node, const FName SocketName);
	void ProcessChildNodes(USceneComponent* NodeParentComponent, FglTFRuntimeNode& Node);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "glTFRuntimeAsset.h"
#include "RpmLoaderComponent.h"
#include "RpmActor.generated.h"

UCLASS()
class RPMNEXTGEN_API ARpmActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARpmActor();

protected:
	virtual void BeginPlay() override;

	template<typename T>
	FName GetSafeNodeName(const FglTFRuntimeNode& Node)
	{
		return MakeUniqueObjectName(this, T::StaticClass(), *Node.Name);
	}

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me")
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me")
	FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me")
	FRpmAnimationCharacter AnimationCharacter;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadGltfAssets(TMap<FString, UglTFRuntimeAsset*> GltfAssetsByType);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadGltfAsset(UglTFRuntimeAsset* GltfAsset, const FString& AssetType = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadGltfAssetWithSkeleton(UglTFRuntimeAsset* GltfAsset, const FString& AssetType, const FRpmAnimationCharacter& InAnimationCharacter);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void RemoveAllMeshes();
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void RemoveMeshComponentsOfType(const FString& AssetType);

protected:
	TWeakObjectPtr<USkeletalMeshComponent> MasterPoseComponent;
private:
	TArray<USceneComponent*> LoadMeshComponents(UglTFRuntimeAsset* GltfAsset);
	USkeletalMeshComponent* CreateSkeletalMeshComponent(UglTFRuntimeAsset* GltfAsset, const FglTFRuntimeNode& Node);
	UStaticMeshComponent* CreateStaticMeshComponent(UglTFRuntimeAsset* GltfAsset, const FglTFRuntimeNode& Node);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Ready Player Me")
	USceneComponent* AssetRoot;
	TMap<FString, TArray<USceneComponent*>> LoadedMeshComponentsByAssetType;
};

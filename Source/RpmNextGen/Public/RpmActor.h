// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "glTFRuntimeAsset.h"
#include "RpmCharacterTypes.h"
#include "RpmLoaderComponent.h"
#include "RpmActor.generated.h"

UCLASS()
class RPMNEXTGEN_API ARpmActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARpmActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me")
	FRpmAnimationConfig AnimationConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me")
	TMap<FString, FRpmAnimationConfig> AnimationConfigsByBaseModelId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|Glb Import Settings")
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "Ready Player Me|Glb Import Settings")
	FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig;

	FRpmCharacterData CharacterData;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadCharacter(const FRpmCharacterData& InCharacterData, UglTFRuntimeAsset* GltfAsset);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadAsset(const FAsset& Asset, UglTFRuntimeAsset* GltfAsset );
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadGltfAssetWithSkeleton(UglTFRuntimeAsset* GltfAsset, const FAsset& Asset, const FRpmAnimationConfig& InAnimationCharacter);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void RemoveAllMeshes();
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void RemoveMeshComponentsOfType(const FString& AssetType);
	
	virtual void Tick(float DeltaTime) override;

protected:
	TWeakObjectPtr<USkeletalMeshComponent> MasterPoseComponent;
	
	virtual void BeginPlay() override;

	template<typename T>
	FName GetSafeNodeName(const FglTFRuntimeNode& Node)
	{
		return MakeUniqueObjectName(this, T::StaticClass(), *Node.Name);
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Ready Player Me")
	USceneComponent* AssetRoot;

	TMap<FString, TArray<USceneComponent*>> LoadedMeshComponentsByAssetType;
	
	TArray<USceneComponent*> LoadMeshComponents(UglTFRuntimeAsset* GltfAsset, const FString& AssetType);
	USkeletalMeshComponent* CreateSkeletalMeshComponent(UglTFRuntimeAsset* GltfAsset, const FglTFRuntimeNode& Node);
	UStaticMeshComponent* CreateStaticMeshComponent(UglTFRuntimeAsset* GltfAsset, const FglTFRuntimeNode& Node);

};

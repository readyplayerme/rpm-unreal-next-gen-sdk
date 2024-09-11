// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RpmAssetLoaderComponent.generated.h"

class UglTFRuntimeAsset;
class FGlbLoader;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssetLoaded, UglTFRuntimeAsset*, Asset, const FString& , AssetType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPMNEXTGEN_API URpmAssetLoaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URpmAssetLoaderComponent();
	
	virtual void LoadCharacterFromUrl(FString Url);
	
	UPROPERTY(BlueprintAssignable, Category = "Ready Player Me" )
	FOnAssetLoaded OnAssetLoaded;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void HandleGLtfAssetLoaded(UglTFRuntimeAsset* gltfAsset, const FString& AssetType);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	TSharedPtr<FGlbLoader> AssetLoader;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UEditorAssetNamer.generated.h"

/**
 * 
 */
UCLASS()
class RPMNEXTGENEDITOR_API UUEditorAssetNamer : public UObject
{
	GENERATED_BODY()

public:
	void SetPath(FString path);
	UFUNCTION()
	FString GenerateMaterialName(UMaterialInterface* Material, int32 MaterialIndex, const FString& SlotName);
	UFUNCTION()
	FString GenerateTextureName(UTexture* Texture, UMaterialInterface* Material, const FString& MaterialPath,
	                            const FString& ParamName);

private:
	FString path;
};

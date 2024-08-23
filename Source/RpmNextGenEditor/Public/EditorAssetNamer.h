// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TransientObjectSaverLibrary.h"
#include "UObject/Object.h"
#include "EditorAssetNamer.generated.h"

/**
 * 
 */
UCLASS()
class RPMNEXTGENEDITOR_API UEditorAssetNamer : public UObject
{
	GENERATED_BODY()

public:
	UEditorAssetNamer();
	void SetPath(FString path);
	UFUNCTION()
	FString GenerateMaterialName(UMaterialInterface* Material, int32 MaterialIndex, const FString& SlotName) const;
	UFUNCTION()
	FString GenerateTextureName(UTexture* Texture, UMaterialInterface* Material, const FString& MaterialPath,
	                            const FString& ParamName) const;

	FTransientObjectSaverMaterialNameGenerator MaterialNameGeneratorDelegate;
	FTransientObjectSaverTextureNameGenerator TextureNameGeneratorDelegate;

private:
	FString path;
};

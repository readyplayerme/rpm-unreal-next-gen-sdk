// Fill out your copyright notice in the Description page of Project Settings.


#include "UEditorAssetNamer.h"

void UUEditorAssetNamer::SetPath(FString NewPath)
{
	this->path = NewPath;
}

FString UUEditorAssetNamer::GenerateMaterialName(UMaterialInterface* Material, const int32 MaterialIndex,
                                                 const FString& SlotName)
{
	return path + TEXT("Material_") + FString::FromInt(MaterialIndex);
}

FString UUEditorAssetNamer::GenerateTextureName(UTexture* Texture, UMaterialInterface* Material,
                                                const FString& MaterialPath, const FString& ParamName)
{
	return path + Material->GetName() + ParamName;
}

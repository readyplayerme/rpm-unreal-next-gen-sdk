// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetNameGenerator.h"

UAssetNameGenerator::UAssetNameGenerator()
{
	MaterialNameGeneratorDelegate.BindUFunction(this, "GenerateMaterialName");
	TextureNameGeneratorDelegate.BindUFunction(this, "GenerateTextureName");
}

void UAssetNameGenerator::SetPath(FString NewPath)
{
	this->path = NewPath;
}

FString UAssetNameGenerator::GenerateMaterialName(UMaterialInterface* Material, const int32 MaterialIndex, const FString& SlotName) const
{
	return FString::Printf(TEXT("%sMaterial_%d"), *path, MaterialIndex);
}

FString UAssetNameGenerator::GenerateTextureName(UTexture* Texture, UMaterialInterface* Material, const FString& MaterialPath, const FString& ParamName) const
{
	return FString::Printf(TEXT("%s%s%s"), *path, *Material->GetName(), *ParamName);
}
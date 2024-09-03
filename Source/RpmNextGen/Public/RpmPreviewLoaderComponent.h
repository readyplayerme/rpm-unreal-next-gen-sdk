// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RpmAssetLoaderComponent.h"
#include "Api/Characters/Models/RpmCharacter.h"
#include "RpmPreviewLoaderComponent.generated.h"

class FCharacterApi;
struct FCharacterCreateResponse;
struct FCharacterUpdateResponse;
struct FCharacterFindByIdResponse;
struct FAsset;

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPMNEXTGEN_API URpmPreviewLoaderComponent : public URpmAssetLoaderComponent
{
	GENERATED_BODY()

public:
	URpmPreviewLoaderComponent();

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void CreateCharacter(const FString& BaseModelId);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadCharacter(FRpmCharacter CharacterData);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadAssetPreview(FAsset AssetData);

	UFUNCTION()
	virtual void HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful);

protected:
	FString AppId;
	FRpmCharacter Character;
	TMap<FString, FString> PreviewAssetMap;
private:
	TSharedPtr<FCharacterApi> CharacterApi;
};

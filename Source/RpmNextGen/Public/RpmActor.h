// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeParser.h"
#include "Api/Assets/Models/Asset.h"
#include "Api/Characters/Models/CharacterCreateResponse.h"
#include "Api/Characters/Models/CharacterFindByIdResponse.h"
#include "Api/Characters/Models/CharacterUpdateResponse.h"
#include "RpmActor.generated.h"

class IHttpResponse;
class IHttpRequest;
class FCharacterApi;
struct FRpmCharacter;
class URpmDeveloperSettings;
class UglTFRuntimeAsset;

/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API ARpmActor : public AActor
{
	GENERATED_BODY()
public:

	ARpmActor();

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	USkeletalMeshComponent* CreateSkeletalMeshComponent(USkeletalMesh* SkeletalMesh, const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadglTFAsset(UglTFRuntimeAsset *Asset, const FString& AssetName);

	void OnAssetDataLoaded(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bIsSuccessful);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadAsset(FAsset AssetData);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadCharacterUrl(FString Url);
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void LoadCharacter(FRpmCharacter CharacterData);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ready Player Me");
	USkeletalMeshComponent* BaseSkeletalMeshComponent;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	void CreateCharacter(FString AppId);
protected:
	FRpmCharacter Character;

	
	TArray<USkeletalMeshComponent*> AddedMeshComponents;

	UFUNCTION()
	virtual void HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleSkeletalMeshLoaded(USkeletalMesh* SkeletalMesh);
	TMap<FString, FString> PreviewAssetMap;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Ready Player Me")
	USceneComponent* AssetRoot;
	URpmDeveloperSettings* RpmSettings;
	FglTFRuntimeSkeletalMeshAsync OnSkeletalMeshCallback;
	TSharedPtr<FCharacterApi> CharacterApi;
};

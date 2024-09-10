// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Api/Characters/Models/RpmCharacter.h"
#include "Components/ActorComponent.h"
#include "RpmLoaderComponent.generated.h"

struct FCharacterCreateResponse;
struct FCharacterUpdateResponse;
struct FCharacterFindByIdResponse;
class FCharacterApi;
struct FAsset;

USTRUCT(BlueprintType)
struct RPMNEXTGEN_API FRpmCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me")
	FString BaseModelId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "assets"))
	TMap<FString, FString> Assets;

	FRpmCharacterData()
	{
		Id = "";
		BaseModelId = "";
		Assets = TMap<FString, FString>();
	}

	FRpmCharacterData(FRpmCharacter Character)
	{
		Id = Character.Id;
		Assets = Character.Assets;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCreated, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterUpdated, FRpmCharacterData, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterFound, FRpmCharacterData, CharacterData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPMNEXTGEN_API URpmLoaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URpmLoaderComponent();
	FOnCharacterCreated OnCharacterCreated;
	FOnCharacterUpdated OnCharacterUpdated;
	FOnCharacterFound OnCharacterFound;
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void CreateCharacter(const FString& BaseModelId, bool bUseCache);
	
	
	UFUNCTION(BlueprintCallable, Category = "Ready Player Me")
	virtual void LoadAssetPreview(FAsset AssetData);


	UFUNCTION()
	virtual void HandleCharacterCreateResponse(FCharacterCreateResponse CharacterCreateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterUpdateResponse(FCharacterUpdateResponse CharacterUpdateResponse, bool bWasSuccessful);
	UFUNCTION()
	virtual void HandleCharacterFindResponse(FCharacterFindByIdResponse CharacterFindByIdResponse, bool bWasSuccessful);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	FString AppId;
	FRpmCharacter Character;
	TMap<FString, FString> PreviewAssetMap;
	FRpmCharacterData CharacterData;
private:
	TSharedPtr<FCharacterApi> CharacterApi;
		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RpmAssetCardWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnImageLoaded, UTexture2D*);

UCLASS()
class RPMNEXTGEN_API URpmAssetCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void Initialize(FAsset Asset);

	void LoadImage(const FString& URL);
};

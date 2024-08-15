// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RpmCategoryPanelWidget.generated.h"

class URpmCategoryButton;
/**
 * 
 */
UCLASS()
class RPMNEXTGEN_API URpmCategoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category Panel")
	TSubclassOf<URpmCategoryButton> SelectedCategoryButton;

	UFUNCTION(BlueprintCallable, Category = "Category Panel")
	void UpdateSelectedButton(TSubclassOf<URpmCategoryButton> CategoryButton);
};

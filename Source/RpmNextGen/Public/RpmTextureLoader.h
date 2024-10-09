#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/Models/Asset.h"
#include "Engine/Texture2D.h"

class FAssetIconLoader;

class RPMNEXTGEN_API FRpmTextureLoader : public TSharedFromThis<FRpmTextureLoader>
{
public:

	FRpmTextureLoader();
	DECLARE_DELEGATE_OneParam(FOnTextureLoaded, UTexture2D*);
	
	void LoadIconFromAsset(const FAsset& Asset, bool bStoreInCache = false);
	UFUNCTION()
	void OnIconLoaded(const FAsset& Asset, const TArray<unsigned char>& Array);

	FOnTextureLoaded OnTextureLoaded;
private:
	TSharedPtr<FAssetIconLoader> AssetIconLoader;
};

#include "RpmTextureLoader.h"

#include "RpmNextGen.h"
#include "Api/Assets/AssetIconLoader.h"
#include "Engine/Texture2D.h"
#include "Utilities/RpmImageHelper.h"

FRpmTextureLoader::FRpmTextureLoader()
{
	AssetIconLoader = MakeShared<FAssetIconLoader>();
	AssetIconLoader->OnIconLoaded.BindRaw( this, &FRpmTextureLoader::OnIconLoaded);
}

void FRpmTextureLoader::LoadIconFromAsset(const FAsset& Asset, bool bStoreInCache)
{
	AssetIconLoader->LoadIcon(Asset, bStoreInCache);
}

void FRpmTextureLoader::OnIconLoaded(const FAsset& Asset, const TArray<unsigned char>& Array)
{
	if (UTexture2D* Texture = FRpmImageHelper::CreateTextureFromData(Array))
	{
		OnTextureLoaded.ExecuteIfBound(Texture);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load icon for asset: %s"), *Asset.Id);
	OnTextureLoaded.ExecuteIfBound(nullptr);
}

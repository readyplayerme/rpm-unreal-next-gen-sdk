#include "Api/Assets/AssetLoader.h"
#include "Api/Assets/Models/Asset.h"
#include "RpmNextGenEditor/Public/EditorAssetLoader.h"

void FAssetLoader::LoadAsset(FAsset* Asset, bool bStoreInCache)
{
	LoadAssetModel(Asset, bStoreInCache);
	LoadAssetImage(Asset, bStoreInCache);
}

void FAssetLoader::FileRequestComplete(TArray<uint8>* Data, FAsset* Asset)
{
}

void FAssetLoader::LoadAssetModel(FAsset* Asset, bool bStoreInCache)
{
	TSharedPtr<FFileApi> FileApi = MakeShareable(new FFileApi());
	TSharedPtr<FAssetLoader> ThisPtr = SharedThis(this);
	FileApi->OnFileRequestComplete.BindLambda([ThisPtr, Asset](TArray<uint8>* Data)
	{
		ThisPtr->FileRequestComplete(Data, Asset);
	});
	FileApi->RequestFromUrl(Asset->GlbUrl);
}

void FAssetLoader::LoadAssetImage(FAsset* Asset, bool bStoreInCache)
{
	TSharedPtr<FFileApi> FileApi = MakeShareable(new FFileApi());
	TSharedPtr<FAssetLoader> ThisPtr = SharedThis(this);
	FileApi->OnFileRequestComplete.BindLambda([ThisPtr, Asset](TArray<uint8>* Data)
	{
		ThisPtr->FileRequestComplete(Data, Asset);
	});
	FileApi->RequestFromUrl(Asset->IconUrl);
}

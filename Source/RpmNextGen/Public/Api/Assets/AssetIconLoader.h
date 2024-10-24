#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"
#include "Models/Asset.h"

struct FAssetLoadingContext;
struct FCachedAssetData;
class IHttpResponse;
class FHttpModule;
struct FAsset;

class RPMNEXTGEN_API FAssetIconLoader : public FWebApi
{
public:
	DECLARE_DELEGATE_TwoParams(FOnIconLoaded, const FAsset&, const TArray<uint8>&);
	
	FOnIconLoaded OnIconLoaded;
	
	FAssetIconLoader();
	virtual ~FAssetIconLoader();
	
	void LoadIcon(const FAsset& Asset, bool bStoreInCache);

private:
	FHttpModule* Http;
};

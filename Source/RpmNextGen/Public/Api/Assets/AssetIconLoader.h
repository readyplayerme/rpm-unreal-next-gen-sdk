#pragma once

#include "CoreMinimal.h"
#include "Models/Asset.h"

struct FAssetLoadingContext;
struct FCachedAssetData;
class IHttpResponse;
class FHttpModule;
struct FAsset;

class RPMNEXTGEN_API FAssetIconLoader : public TSharedFromThis<FAssetIconLoader>
{
public:
	DECLARE_DELEGATE_TwoParams(FOnIconLoaded, const FAsset&, const TArray<uint8>&);

	FAssetIconLoader();
	virtual ~FAssetIconLoader();
	void LoadIcon(const FAsset& Asset, bool bStoreInCache);

	FOnIconLoaded OnIconLoaded;

private:
	void LoadIcon(TSharedRef<FAssetLoadingContext> Context);
	void IconLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);

	FHttpModule* Http;
	
};

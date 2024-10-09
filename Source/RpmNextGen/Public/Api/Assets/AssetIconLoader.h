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
	
	FOnIconLoaded OnIconLoaded;
	
	FAssetIconLoader();
	virtual ~FAssetIconLoader();
	
	void LoadIcon(const FAsset& Asset, bool bStoreInCache);

private:
	FHttpModule* Http;
	
	UFUNCTION()
	void IconLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);

	void LoadIcon(TSharedRef<FAssetLoadingContext> Context);
};

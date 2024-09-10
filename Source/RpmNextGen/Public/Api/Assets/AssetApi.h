﻿#pragma once
#include "Api/Common/WebApiWithAuth.h"
#include "Models/AssetTypeListResponse.h"

struct FAssetTypeListRequest;
struct FAssetListRequest;
struct FAssetListResponse;

DECLARE_DELEGATE_TwoParams(FOnListAssetsResponse, const FAssetListResponse&, bool);
DECLARE_DELEGATE_TwoParams(FOnListAssetTypeResponse, const FAssetTypeListResponse&, bool);

class RPMNEXTGEN_API FAssetApi : public FWebApiWithAuth
{
public:
	FAssetApi();
	void ListAssetsAsync(const FAssetListRequest& Request);
	void ListAssetTypesAsync(const FAssetTypeListRequest& Request);
	FOnListAssetsResponse OnListAssetsResponse;
	FOnListAssetTypeResponse OnListAssetTypeResponse;
	static const FString BaseModelType;
private:
	void HandleResponse(FString Response, bool bWasSuccessful);
	FString ApiBaseUrl;
};

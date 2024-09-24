#pragma once

#include "CoreMinimal.h"
#include "RpmNextGen.h"
#include "Interfaces/IHttpRequest.h"

DECLARE_DELEGATE_ThreeParams(FOnFileRequestComplete, TArray<uint8>*, const FString&, const FString&);

class RPMNEXTGEN_API FFileApi : public TSharedFromThis<FFileApi>
{
public:
	FFileApi();
	virtual ~FFileApi();
	virtual void LoadFileFromUrl(const FString& URL, const FString& AssetType = TEXT(""));
	virtual void FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString AssetType);
	static bool LoadFileFromPath(const FString& Path, TArray<uint8>& OutContent);

	FOnFileRequestComplete OnFileRequestComplete;
};

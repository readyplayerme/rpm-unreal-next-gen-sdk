#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

struct FAsset;
DECLARE_DELEGATE_TwoParams(FOnFileRequestComplete, TArray<uint8>*, const FString&);

class RPMNEXTGEN_API FFileApi : public TSharedFromThis<FFileApi>
{
public:
	FFileApi();
	virtual ~FFileApi();
	virtual void LoadFileFromUrl(const FString& URL);
	virtual void FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	static bool LoadFileFromPath(const FString& Path, TArray<uint8>& OutContent);

	FOnFileRequestComplete OnFileRequestComplete;
};

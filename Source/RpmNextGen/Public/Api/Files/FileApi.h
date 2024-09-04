#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

DECLARE_DELEGATE_OneParam(FOnFileRequestComplete, TArray<uint8>*);

class RPMNEXTGEN_API FFileApi : public TSharedFromThis<FFileApi>
{
public:
	FFileApi();
	virtual ~FFileApi();
	virtual void RequestFromUrl(const FString& URL);
	virtual void FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FOnFileRequestComplete OnFileRequestComplete;
};

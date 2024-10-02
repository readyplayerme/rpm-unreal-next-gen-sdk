#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Api/Common/WebApi.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Models/CharacterCreateRequest.h"
#include "Models/CharacterCreateResponse.h"
#include "Models/CharacterFindByIdRequest.h"
#include "Models/CharacterFindByIdResponse.h"
#include "Models/CharacterPreviewRequest.h"
#include "Models/CharacterUpdateRequest.h"
#include "Models/CharacterUpdateResponse.h"

DECLARE_DELEGATE_TwoParams(FOnCharacterCreateResponse, FCharacterCreateResponse, bool);
DECLARE_DELEGATE_TwoParams(FOnCharacterUpdatResponse, FCharacterUpdateResponse, bool);
DECLARE_DELEGATE_TwoParams(FOnCharacterFindResponse, FCharacterFindByIdResponse, bool);

class RPMNEXTGEN_API FCharacterApi : public TSharedFromThis<FCharacterApi, ESPMode::ThreadSafe>, public FWebApiWithAuth
{
public:
	FOnRequestComplete OnApiResponse;
	FOnCharacterCreateResponse OnCharacterCreateResponse;
	FOnCharacterUpdatResponse OnCharacterUpdateResponse;
	FOnCharacterFindResponse OnCharacterFindResponse;
	
	FCharacterApi();
	virtual ~FCharacterApi() override;

	void CreateAsync(const FCharacterCreateRequest& Request);
	void UpdateAsync(const FCharacterUpdateRequest& Request);
	void FindByIdAsync(const FCharacterFindByIdRequest& Request);
	FString GeneratePreviewUrl(const FCharacterPreviewRequest& Request);

protected:
	FHttpModule* Http;
	
	template <typename T>
	FString ConvertToJsonString(const T& Data);

	virtual void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;
	void CreateCharacterFromCache();
private:
	FString BaseUrl;
	TMap<FString, FString> AssetByType = TMap<FString, FString>();
};

template <typename T>
FString FCharacterApi::ConvertToJsonString(const T& Data)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	return JsonString;
}

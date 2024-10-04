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

class RPMNEXTGEN_API FCharacterApi : public FWebApiWithAuth
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
	template <typename T>
	FString ConvertToJsonString(const T& Data);


	void HandleCharacterResponse(const FApiRequest& ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleCharacterCreateResponse(FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleUpdateResponse( FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleFindResponse(FHttpResponsePtr Response, bool bWasSuccessful);
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

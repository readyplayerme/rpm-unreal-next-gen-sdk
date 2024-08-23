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

DECLARE_LOG_CATEGORY_EXTERN(ReadyPlayerMe, Log, All);

UENUM(BlueprintType)
enum ECharacterResponseType
{
	Create,
	Update,
	FindById
};

DECLARE_DELEGATE_TwoParams(FOnCharacterCreateResponse, FCharacterCreateResponse, bool);
DECLARE_DELEGATE_TwoParams(FOnCharacterUpdatResponse, FCharacterUpdateResponse, bool);
DECLARE_DELEGATE_TwoParams(FOnCharacterFindResponse, FCharacterFindByIdResponse, bool);

class RPMNEXTGEN_API FCharacterApi : public TSharedFromThis<FCharacterApi, ESPMode::ThreadSafe>, public FWebApiWithAuth
{
public:
	FCharacterApi();
	virtual ~FCharacterApi() override;
	FOnWebApiResponse OnApiResponse;

	void CreateAsync(const FCharacterCreateRequest& Request);
	void UpdateAsync(const FCharacterUpdateRequest& Request);
	void FindByIdAsync(const FCharacterFindByIdRequest& Request);
	FString GeneratePreviewUrl(const FCharacterPreviewRequest& Request);

	FOnCharacterCreateResponse OnCharacterCreateResponse;
	FOnCharacterUpdatResponse OnCharacterUpdateResponse;
	FOnCharacterFindResponse OnCharacterFindResponse;

protected:
	void Dispatch(FApiRequest Data, const ECharacterResponseType& ResponseType);

	void OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;

	FString BuildQueryString(const TMap<FString, FString>& QueryParams);

	template <typename T>
	FString ConvertToJsonString(const T& Data);

	FHttpModule* Http;

private:
	FString BaseUrl;
};

template <typename T>
FString FCharacterApi::ConvertToJsonString(const T& Data)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Data, JsonString);
	return JsonString;
}

#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Models/CharacterCreateRequest.h"
#include "Models/CharacterCreateResponse.h"
#include "Models/CharacterFindByIdRequest.h"
#include "Models/CharacterFindByIdResponse.h"
#include "Models/CharacterPreviewRequest.h"
#include "Models/CharacterUpdateRequest.h"
#include "Models/CharacterUpdateResponse.h"

DECLARE_DELEGATE_TwoParams(FOnCharacterCreateResponse, TSharedPtr<FCharacterCreateResponse>, bool);
DECLARE_DELEGATE_TwoParams(FOnCharacterUpdatResponse, TSharedPtr<FCharacterUpdateResponse>, bool);
DECLARE_DELEGATE_TwoParams(FOnCharacterFindResponse, TSharedPtr<FCharacterFindByIdResponse>, bool);

class RPMNEXTGEN_API FCharacterApi : public FWebApiWithAuth
{	
public:
	
	FCharacterApi();
	virtual ~FCharacterApi() override;
	
	void CreateAsync(TSharedPtr<FCharacterCreateRequest> Request, FOnCharacterCreateResponse OnComplete);
	void UpdateAsync(TSharedPtr<FCharacterUpdateRequest> Request, FOnCharacterUpdatResponse OnComplete);
	void FindByIdAsync(TSharedPtr<FCharacterFindByIdRequest> Request, FOnCharacterFindResponse OnComplete);
	FString GeneratePreviewUrl(const FCharacterPreviewRequest& Request);

private:
	FString BaseUrl;
	TMap<FString, FString> AssetByType = TMap<FString, FString>();
};

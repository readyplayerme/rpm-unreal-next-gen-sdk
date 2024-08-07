#pragma once

#include "CoreMinimal.h"
#include "DeveloperLoginResponse.h"
#include "DevAuthTokenCache.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FDeveloperAuth
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me",  meta = (JsonName = "token"))
	FString Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "refreshToken"))
	FString RefreshToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "name"))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ready Player Me", meta = (JsonName = "isDemo"))
	bool IsDemo;

	FDeveloperAuth() = default;
	
	FDeveloperAuth(FDeveloperLoginResponseBody Data, bool bIsDemo)
	{
		Token = Data.Token;
		RefreshToken = Data.RefreshToken;
		Name = Data.Name;
		IsDemo = bIsDemo;
	}

	FString ToJsonString() const
	{
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		Writer->WriteObjectStart();
		Writer->WriteValue(TEXT("token"), Token);
		Writer->WriteValue(TEXT("refreshToken"), RefreshToken);
		Writer->WriteValue(TEXT("name"), Name);
		Writer->WriteValue(TEXT("isDemo"), IsDemo);
		Writer->WriteObjectEnd();
		Writer->Close();
		return OutputString;
	}

	static bool FromJsonString(const FString& JsonString, FDeveloperAuth& OutStruct)
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &OutStruct, 0, 0);
	}

	bool IsValid() const
	{
		return !Token.IsEmpty();
	}
};


class RPMNEXTGENEDITOR_API DevAuthTokenCache
{
public:
	static FDeveloperAuth GetAuthData();
	static void SetAuthData(const FDeveloperLoginResponse& Token);
	static void ClearAuthData();
private:
	static constexpr const TCHAR* CacheKeyName = TEXT("Name");
	static constexpr const TCHAR* CacheKeyToken = TEXT("Token");
	static constexpr const TCHAR* CacheKeyRefreshToken = TEXT("RefreshToken");
	static constexpr const TCHAR* CacheKeyIsDemo = TEXT("IsDemo");
	
	static FDeveloperAuth AuthData;
	static bool bIsInitialized;
	static void Initialize();
};

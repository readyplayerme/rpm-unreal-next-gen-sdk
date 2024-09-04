#include "Api/Files/FileApi.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

FFileApi::FFileApi()
{
}

FFileApi::~FFileApi()
{
}

void FFileApi::RequestFromUrl(const FString& URL)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileApi::FileRequestComplete);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

void FFileApi::FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
	{
		TArray<uint8> Content = Response->GetContent();
		OnFileRequestComplete.ExecuteIfBound(&Content);
		return;
	}
	OnFileRequestComplete.ExecuteIfBound(nullptr);
}

#include "ConnectionManager.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ScopeLock.h"

FConnectionManager& FConnectionManager::Get()
{
    // Singleton instance of the connection manager
    static FConnectionManager Instance;
    return Instance;
}

FConnectionManager::FConnectionManager()
{
}

FConnectionManager::~FConnectionManager()
{
}

bool FConnectionManager::IsConnected()
{
    // Lock access to ensure thread safety
    FScopeLock Lock(&ConnectionStatusCriticalSection);
    return bIsConnected;
}

void FConnectionManager::CheckInternetConnection()
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://www.google.com")); 
    Request->SetVerb(TEXT("GET"));
    Request->OnProcessRequestComplete().BindRaw(this, &FConnectionManager::OnCheckResponse);
    
    Request->ProcessRequest();
}

void FConnectionManager::OnCheckResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    const bool bNewConnectionStatus = bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200;

    UpdateConnectionStatus(bNewConnectionStatus);

    UE_LOG(LogReadyPlayerMe, Log, TEXT("Connection check result: %s"), bIsConnected ? TEXT("Connected") : TEXT("Disconnected"));
}

void FConnectionManager::UpdateConnectionStatus(bool bNewStatus)
{
    FScopeLock Lock(&ConnectionStatusCriticalSection);
    
    if (bNewStatus != bIsConnected)
    {
        bIsConnected = bNewStatus;
        OnConnectionStatusChanged.ExecuteIfBound(bIsConnected);
    }
}

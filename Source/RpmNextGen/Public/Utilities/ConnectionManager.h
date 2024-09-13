#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/ScopeLock.h"

DECLARE_DELEGATE_OneParam(FOnConnectionStatusChanged, bool);
DECLARE_DELEGATE_OneParam(FOnConnectionStatusRefreshed, bool);

/// <summary>
/// A Singleton class that checks and tracks internet connection status.
/// </summary>
class RPMNEXTGEN_API FConnectionManager
{
public:
	static FConnectionManager& Get();

	virtual ~FConnectionManager();

	bool IsConnected();

	void CheckInternetConnection();

	FOnConnectionStatusChanged OnConnectionStatusChanged;
	FOnConnectionStatusRefreshed OnConnectionStatusRefreshed;

private:
	FConnectionManager();

	void OnCheckResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void UpdateConnectionStatus(bool bNewStatus);
	
	bool bIsConnected = true;

	FCriticalSection ConnectionStatusCriticalSection;
};

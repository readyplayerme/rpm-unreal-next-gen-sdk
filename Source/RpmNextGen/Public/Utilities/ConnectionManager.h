#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/ScopeLock.h"

// Declare a delegate to notify when connection status changes
DECLARE_DELEGATE_OneParam(FOnConnectionStatusChanged, bool);

/**
 * A Singleton class that checks and tracks internet connection status.
 */
class RPMNEXTGEN_API FConnectionManager
{
public:
	// Singleton access to the Connection Manager
	static FConnectionManager& Get();

	// Destructor
	virtual ~FConnectionManager();

	// Returns the current connection status
	bool IsConnected();

	// Initiates a check for internet connection
	void CheckInternetConnection();

	// Delegate to notify any subscribers about connection status changes
	FOnConnectionStatusChanged OnConnectionStatusChanged;

private:
	// Constructor (private as part of Singleton pattern)
	FConnectionManager();

	// Response handler for HTTP connection check
	void OnCheckResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Update the connection status and notify listeners
	void UpdateConnectionStatus(bool bNewStatus);

	// Connection status flag
	bool bIsConnected = false;

	// Critical section to ensure thread safety
	FCriticalSection ConnectionStatusCriticalSection;
};

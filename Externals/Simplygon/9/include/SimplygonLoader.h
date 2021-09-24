#ifndef __SIMPLYGONLOADER_H__
#define __SIMPLYGONLOADER_H__

#include "Simplygon.h"
#include <tchar.h>
#include <WTypes.h>
#include <cstdint>

namespace Simplygon {

class MemoryStatistics
{
	public:
	uint64_t AllocatedObjects;
	uint64_t AllocatedBytes;
	uint64_t Unused[2];
};

/// Adds a location to look for the DLL in. This method must be called before calling Initialize()
void AddSearchPath( const char* search_path );

/// Clears the additional search locations to look for the DLL in.
void ClearAdditionalSearchPaths();

/// Loads and initializes the SDK
EErrorCodes Initialize( ISimplygon** pInterfacePtr, const char* SDKPath = nullptr, LPCSTR LicenseDataText = nullptr );

/// Deinitializes the SDK, releases the DLL and all allocated memory
void Deinitialize( Simplygon::ISimplygon* pSimplygonInterface );

/// Perform thread cleanup, call at end of thread function if thread lifetime is not known (as in async thread pools)
void DeinitializeThread();

/// Get memory statistics
MemoryStatistics GetMemoryStatistics();

/// Retrieves the error string of the error code.
const char* GetError( EErrorCodes error_code );

/// Retrieves the license system log, for when initializing
int PollLog( char* dest, int max_len_dest );

/// Return path to license file used to initialize Simplygon in last successful call to Initialize or null if no license file found or used
const char* GetLicenseFilePath();

/// Return file name of license file
const char* GetLicenseFileName();

/// Return true if license is nodelocked
bool IsLicenseNodelocked();

};

#endif //__SIMPLYGONLOADER_H__

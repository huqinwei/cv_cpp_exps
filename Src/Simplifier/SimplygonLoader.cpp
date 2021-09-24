#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tchar.h>
#include <windows.h>
#include <ShlObj.h>
#include "Simplifier/SimplygonLoader.h"

#ifndef _WIN64
#error Simplygon only supports Windows x64 builds
#endif

#ifdef __cpp_lib_filesystem // Check for C++17 filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#ifndef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#endif

static std::vector<fs::path> AdditionalSearchPaths;

using LPINITIALIZESimplygon = int( CALLBACK* )( const char* license_data, Simplygon::ISimplygon** pInterfacePtr );
using LPDEINITIALIZESimplygon = void( CALLBACK* )( Simplygon::ISimplygon* pSimplygonInterface );
using LPDEINITIALIZESimplygonThread = void( CALLBACK* )();
using LPGETINTERFACEVERSIONSimplygon = void( CALLBACK* )( char* deststring );
using LPPOLLLOGSimplygon = int( CALLBACK* )( char* destbuffer, int max_length );
using LPISSimplygonLicenseNodelocked = bool( CALLBACK* )( void );
using LPGETSimplygonMemoryStatistics = void( CALLBACK* )( Simplygon::MemoryStatistics*, size_t );

namespace Simplygon {

static LPINITIALIZESimplygon InitializeSimplygonPtr = nullptr;
static LPDEINITIALIZESimplygon DeinitializeSimplygonPtr = nullptr;
static LPDEINITIALIZESimplygonThread DeinitializeSimplygonThreadPtr = nullptr;
static LPGETINTERFACEVERSIONSimplygon GetInterfaceVersionSimplygonPtr = nullptr;
static LPGETSimplygonMemoryStatistics GetMemoryStatisticsPtr = nullptr;
static LPPOLLLOGSimplygon PollLogSimplygonPtr = nullptr;
static LPISSimplygonLicenseNodelocked IsSimplygonLicenseNodelockedPtr = nullptr;
static HINSTANCE libHandle = nullptr;                                                // Handle to Simplygon DLL
static Simplygon::EErrorCodes LoadError = Simplygon::EErrorCodes::InvalidInputParam; // if the load failed, this contains the error

static constexpr char SIMPLYGON_RELEASE_DLL[] = "Simplygon.dll";
static constexpr char SIMPLYGON_DEBUGRELEASE_DLL[] = "SimplygonDebugRelease.dll";
static constexpr char SIMPLYGON_DEBUG_DLL[] = "SimplygonDebug.dll";
static constexpr char LICENSE_FILE_NAME[] = "Simplygon_license.dat";
static bool LicenseIsNodelocked;

// CSIDL_COMMON_APPDATA
// CSIDL_LOCAL_APPDATA
static fs::path GetSpecialFolder( int csidl )
{
	char FolderPath[ MAX_PATH ];

	// Get the common appdata path
	if( SHGetFolderPathA( nullptr, csidl, nullptr, 0, FolderPath ) != 0 )
		return { "" };

	// append the path to Simplygon SDK
	return fs::path( FolderPath ) / "Microsoft" / "SimplygonSDK";
}

static std::wstring GetEnvVar( const wchar_t* var )
{
	std::wstring val;

	wchar_t envBuffer[ ( _MAX_PATH + 2 ) * 2 ];
	wchar_t* envPath = envBuffer;
	wchar_t* expanded = nullptr;
	size_t envCap = sizeof( envBuffer );
	DWORD envLen = GetEnvironmentVariableW( var, envPath, (DWORD)envCap );
	if( envLen >= sizeof( envBuffer ) )
	{
		envCap = ( (size_t)envLen + 2 ) * 2;
		envPath = new wchar_t[ envCap ];
		DWORD gotLen = GetEnvironmentVariableW( var, envPath, (DWORD)envCap );
		if( !gotLen || ( gotLen >= envCap ) )
			envLen = 0;
		else
			envLen = gotLen;
	}
	if( envLen )
	{
		envPath[ envLen ] = 0;

		envCap += 2;
		expanded = new wchar_t[ envCap ];
		DWORD expandedLen = ExpandEnvironmentStringsW( envPath, expanded, (DWORD)envCap );
		if( expandedLen && ( expandedLen <= envCap ) )
			val = expanded;
	}

	delete[] expanded;
	if( envPath != envBuffer )
		delete[] envPath;

	return val;
}

static fs::path GetEnvPath( const wchar_t* var )
{
	return fs::path( GetEnvVar( var ) );
}

static fs::path GetEnvSimplygonPath()
{
	return GetEnvPath( L"SIMPLYGON_9_PATH" );
}

static fs::path GetEnvTempPath()
{
	return GetEnvPath( L"SIMPLYGON_9_TEMP" );
}

static fs::path GetApplicationPath()
{
	char Path[ MAX_PATH ];
	if( GetModuleFileNameA( nullptr, Path, MAX_PATH ) == 0 )
		return { "" };

	return fs::path( Path ).parent_path();
}

static std::vector<fs::path> GetAllSearchPaths()
{
	std::vector<fs::path> paths = AdditionalSearchPaths;
	paths.push_back( fs::current_path() );
	paths.push_back( GetApplicationPath() );
	paths.push_back( GetEnvSimplygonPath() );
	return paths;
}

static bool DynamicLibraryIsLoaded()
{
	if( libHandle == nullptr )
		return false;

	return true;
}

static void UnloadDynamicLibrary()
{
	if( !DynamicLibraryIsLoaded() )
		return;

	FreeLibrary( libHandle );
	libHandle = nullptr;

	InitializeSimplygonPtr = nullptr;
	DeinitializeSimplygonPtr = nullptr;
	DeinitializeSimplygonThreadPtr = nullptr;
	GetInterfaceVersionSimplygonPtr = nullptr;
	PollLogSimplygonPtr = nullptr;
	IsSimplygonLicenseNodelockedPtr = nullptr;

	LicenseIsNodelocked = false;
}

static bool LoadOSLibrary( const fs::path& lib_path )
{
	// if no file exists, just return
	if( !fs::exists( lib_path ) )
	{
		LoadError = EErrorCodes::DLLOrDependenciesNotFound;
		return false;
	}

	// try loading the library
	libHandle = LoadLibraryA( lib_path.u8string().c_str() );

	// now, check if the library was loaded
	if( !DynamicLibraryIsLoaded() || libHandle == nullptr )
	{
		// load failed, probably a missing dependency
		DWORD err = GetLastError();
		std::stringstream buffer;
		buffer << "Loading the Simplygon DLL failed, which is in most cases because of a missing dependency, are all dependencies installed? Please try "
		          "reinstalling Simplygon. GetLastError returns "
		       << err << std::endl;
		OutputDebugStringA( buffer.str().c_str() );
		// set the error
		LoadError = Simplygon::EErrorCodes::DLLFailedToLoad;
		return false;
	}

	// setup the pointers
	InitializeSimplygonPtr = (LPINITIALIZESimplygon)GetProcAddress( libHandle, "InitializeSimplygon" );
	DeinitializeSimplygonPtr = (LPDEINITIALIZESimplygon)GetProcAddress( libHandle, "DeinitializeSimplygon" );
	DeinitializeSimplygonThreadPtr = (LPDEINITIALIZESimplygonThread)GetProcAddress( libHandle, "DeinitializeSimplygonThread" );
	GetMemoryStatisticsPtr = (LPGETSimplygonMemoryStatistics)GetProcAddress( libHandle, "GetSimplygonMemoryStatistics" );
	GetInterfaceVersionSimplygonPtr = (LPGETINTERFACEVERSIONSimplygon)GetProcAddress( libHandle, "GetInterfaceVersionSimplygon" );
	PollLogSimplygonPtr = (LPPOLLLOGSimplygon)GetProcAddress( libHandle, "PollLogSimplygon" );
	IsSimplygonLicenseNodelockedPtr = (LPISSimplygonLicenseNodelocked)GetProcAddress( libHandle, "IsSimplygonLicenseNodelocked" );
	if( InitializeSimplygonPtr == nullptr || DeinitializeSimplygonPtr == nullptr || DeinitializeSimplygonThreadPtr == nullptr || GetInterfaceVersionSimplygonPtr == nullptr )
	{
		// load failed, invalid version
		std::stringstream buffer;
		buffer << "The Simplygon DLL loaded, but the .dll functions could not be retrieved, this is probably not a Simplygon dll file, or it is corrupted. "
		          "Please reinstall Simplygon"
		       << std::endl;
		OutputDebugStringA( buffer.str().c_str() );
		// set error
		LoadError = Simplygon::EErrorCodes::DLLAndHeaderFileIsNotMatching;
		UnloadDynamicLibrary();
		return false;
	}

	// check the version string
	char versionstring[ 200 ];
	GetInterfaceVersionSimplygonPtr( versionstring );
	std::string header_version( Simplygon::GetInterfaceVersionHash() );

	std::stringstream buffer;
	if( !header_version.empty() && (header_version != versionstring) )
	{
		// load failed, invalid version
		buffer << "The Simplygon DLL loaded, but the interface version of the header is not the same as the library or invalid." << std::endl
		       << "\tHeader : " << header_version << std::endl
		       << "\tLibrary: " << versionstring << std::endl;
		OutputDebugStringA( buffer.str().c_str() );
		LoadError = Simplygon::EErrorCodes::DLLAndHeaderFileIsNotMatching;
		UnloadDynamicLibrary();
		return false;
	}

	// loaded successfully
	buffer << "The Simplygon DLL was found and loaded successfully!" << std::endl
	       << "\tDLL path: " << lib_path << std::endl
	       << "\tInterface hash: " << versionstring << std::endl;
	OutputDebugStringA( buffer.str().c_str() );
	LoadError = Simplygon::EErrorCodes::NoError;
	return true;
}

static bool LoadNamedDynamicLibrary( const std::string& DLLName )
{
	auto dllSearchPaths = GetAllSearchPaths();
	for( const auto& path : dllSearchPaths )
	{
		if( !path.empty() && LoadOSLibrary( path / DLLName ) )
			return true;
	}

	std::stringstream buffer;
	buffer << "Could not find Simplygon DLL (" << DLLName << ") in search paths:" << std::endl;
	for( const auto& dir : dllSearchPaths )
	{
		if( !dir.empty() )
			buffer << dir.u8string() << std::endl;
	}
	OutputDebugStringA( buffer.str().c_str() );
	return false;
}

static bool LoadDynamicLibrary( const fs::path& SDKPath )
{
	LoadError = Simplygon::EErrorCodes::NoError;

	std::vector<std::string> DLLNames = { SIMPLYGON_RELEASE_DLL, SIMPLYGON_DEBUGRELEASE_DLL, SIMPLYGON_DEBUG_DLL };

#ifdef _DEBUG
	std::reverse( std::begin( DLLNames ), std::end( DLLNames ) );
#elif defined( REDEBUG )
	std::swap( DLLNames[ 1 ], DLLNames[ 0 ] );
#endif

	// load from explicit place, fail if not found
	if( !SDKPath.empty() )
	{
		if( LoadOSLibrary( SDKPath ) )
			return true;

		LoadError = Simplygon::EErrorCodes::DLLOrDependenciesNotFound;
		return false;
	}

	for( auto& DLLName : DLLNames )
	{
		if( LoadNamedDynamicLibrary( DLLName ) )
			return true;
	}

	LoadError = Simplygon::EErrorCodes::DLLOrDependenciesNotFound;
	return false;
}

static fs::path FindNamedProcess( const std::string& EXEName )
{
	std::vector<fs::path> dirs = GetAllSearchPaths();

	// try the different paths
	for( auto& dir : dirs )
	{
		if( fs::is_empty( dir ) )
			continue;

		auto testPath = dir / EXEName;
		if( fs::exists( testPath ) )
			return testPath;
	}

	return { "" };
}

// executes the batch process. redirects the stdout to the handle specified
// places the handle of the process into the variable that phProcess points at
static bool ExecuteProcess( const std::string& exepath, const std::string& ini_file, HANDLE* phProcess )
{
	PROCESS_INFORMATION piProcInfo = { 0 };
	STARTUPINFOA siStartInfo = { 0 };
	BOOL bFuncRetn = FALSE;

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory( &piProcInfo, sizeof( PROCESS_INFORMATION ) );

	// Set up members of the STARTUPINFO structure.
	ZeroMemory( &siStartInfo, sizeof( STARTUPINFOA ) );
	siStartInfo.cb = sizeof( STARTUPINFOA );

	// create a command line string
	std::string tmp_cmdline = exepath + " " + ini_file;

	// Create the child process.
	bFuncRetn = CreateProcessA( tmp_cmdline.c_str(), // exe file path
	                            nullptr,             // command line
	                            nullptr,             // process security attributes
	                            nullptr,             // primary thread security attributes
	                            TRUE,                // handles are inherited
	                            0,                   // creation flags
	                            nullptr,             // use parent's environment
	                            nullptr,             // use parent's current directory
	                            &siStartInfo,        // STARTUPINFO pointer
	                            &piProcInfo          // receives PROCESS_INFORMATION
	);

	if( bFuncRetn == 0 )
		return false;

	// function succeeded, return handle to process, release handles we will not use anymore
	*phProcess = piProcInfo.hProcess;
	if( piProcInfo.hThread != INVALID_HANDLE_VALUE )
		CloseHandle( piProcInfo.hThread );
	return true;
}

// waits for process to end, and returns exit value
// the process handle is also released by the function
static DWORD WaitForProcess( HANDLE& hProcess )
{
	DWORD exitcode = (DWORD)-1;
	if( hProcess == INVALID_HANDLE_VALUE )
		return exitcode;

	for( ;; )
	{
		// check if process has ended
		GetExitCodeProcess( hProcess, &exitcode );
		if( exitcode != STILL_ACTIVE )
			break;

		// wait for it to signal.
		WaitForSingleObject( hProcess, 1000 );
	}

	CloseHandle( hProcess );
	hProcess = INVALID_HANDLE_VALUE;
	return exitcode;
}

static bool ReadLicense( const fs::path& LicensePath, std::string& License )
{
	License.clear();
	if( !fs::exists( LicensePath ) )
		return false;

	std::ifstream filestream( LicensePath );

	if( !filestream.is_open() )
		return false;

	filestream.seekg( 0, std::ios::end );
	auto len = static_cast<size_t>( filestream.tellg() );
	License.reserve( len );
	filestream.seekg( 0, std::ios::beg );
	License.assign( std::istreambuf_iterator<char>( filestream ), std::istreambuf_iterator<char>() );

	filestream.close();
	return true;
}

static char LicenseFilePath[ _MAX_PATH ];

const char* GetLicenseFilePath()
{
	return LicenseFilePath[ 0 ] ? LicenseFilePath : 0;
}

const char* GetLicenseFileName()
{
	return LICENSE_FILE_NAME;
}

static bool LoadLicense( std::string& lic )
{
	LicenseFilePath[ 0 ] = 0;

	std::vector<fs::path> licenseSearchPaths = GetAllSearchPaths();
	licenseSearchPaths.emplace_back( GetSpecialFolder( CSIDL_COMMON_APPDATA ) );
	licenseSearchPaths.emplace_back( GetSpecialFolder( CSIDL_LOCAL_APPDATA ) );

	for( const auto& dir : licenseSearchPaths )
	{
		if( dir.empty() )
			continue;
		auto LicensePath = dir / LICENSE_FILE_NAME;
		if( ReadLicense( LicensePath, lic ) )
		{
			std::string filePath = LicensePath.generic_u8string();
			strncpy_s( LicenseFilePath, filePath.c_str(), filePath.length() );
			return true;
		}
	}

	std::stringstream buffer;
	buffer << "Could not find license file (" << LICENSE_FILE_NAME << ") in search paths:" << std::endl;
	for( const auto& dir : licenseSearchPaths )
	{
		if( !dir.empty() )
			buffer << dir.u8string() << std::endl;
	}
	OutputDebugStringA( buffer.str().c_str() );
	return false;
}

EErrorCodes InitializeDLL( const char* SDKPath )
{
	if( !SDKPath )
		SDKPath = { "" };

	// load the library
	if( !LoadDynamicLibrary( SDKPath ) )
		return LoadError;

	return EErrorCodes::NoError;
}

void DeinitializeDLL()
{
	if( !DynamicLibraryIsLoaded() )
		return;

	UnloadDynamicLibrary();
}

////////////////////////////////////////////////////////////////////////////////////

static int init_count = 0; // a reference count of the number of Init/Deinits

void AddSearchPath( const char* search_path )
{
	fs::path temp_path{ search_path };
	AdditionalSearchPaths.emplace_back( fs::absolute( temp_path ) );
}

void ClearAdditionalSearchPaths()
{
	AdditionalSearchPaths.clear();
}

EErrorCodes Initialize( ISimplygon** pInterfacePtr, const char* SDKPath, const char* LicenseDataText )
{
	EErrorCodes retval = EErrorCodes::NoError;
	if( !DynamicLibraryIsLoaded() )
	{
		retval = InitializeDLL( SDKPath );
		if( retval != EErrorCodes::NoError )
			return retval;
	}
	++init_count;

	// read the license file from the installation path
	std::string lic;
	if( LicenseDataText )
	{
		lic.assign( LicenseDataText );
	}
	else
	{
		if( !LoadLicense( lic ) )
		{
			// Check if fallback feature flag is set
			std::wstring featureFlags = GetEnvVar( L"SIMPLYGON_9_FLAGS" );
			bool fallback = false;
			for( size_t start = 0, delim = featureFlags.find( L';' ); ; start = delim + 1, delim = featureFlags.find( L';', delim + 1 ) )
			{
				if( featureFlags.substr( start, ( delim == std::wstring::npos ) ? delim : delim - start ) == L"NoLicenseFallback" )
				{
					lic.assign( "SimplygonInitNoLicense" );
					fallback = true;
					break;
				}
				if( delim == std::wstring::npos )
					break;
			}
			if( !fallback )
				return EErrorCodes::NoLicense;
		}
	}

	// initialization
	retval = (EErrorCodes)InitializeSimplygonPtr( lic.c_str(), pInterfacePtr );
	if( retval == EErrorCodes::NoError )
	{
		std::stringstream buffer;
		buffer << "Simplygon initialized and running" << std::endl << "\tVersion: " << ( *pInterfacePtr )->GetVersion() << std::endl;
		OutputDebugStringA( buffer.str().c_str() );

		// Check license type
		if( IsSimplygonLicenseNodelockedPtr )
			LicenseIsNodelocked = IsSimplygonLicenseNodelockedPtr();
	}

	return retval;
}

void Deinitialize( Simplygon::ISimplygon* pSimplygonInterface )
{
	if( DynamicLibraryIsLoaded() && DeinitializeSimplygonPtr != nullptr )
	{
		// Release the Simplygon interface
		DeinitializeSimplygonPtr( pSimplygonInterface );

		--init_count;
		// if the reference is less or equal to 0, release the dll interface
		if( init_count <= 0 )
		{
			DeinitializeDLL();
			init_count = 0;
		}
	}
}

void DeinitializeThread()
{
	if( DynamicLibraryIsLoaded() && DeinitializeSimplygonThreadPtr != nullptr )
	{
		DeinitializeSimplygonThreadPtr();
	}
}

MemoryStatistics GetMemoryStatistics()
{
	MemoryStatistics stats;
	memset( &stats, 0, sizeof( stats ) );
	if( DynamicLibraryIsLoaded() && GetMemoryStatisticsPtr != nullptr )
		GetMemoryStatisticsPtr( &stats, sizeof( stats ) );
	return stats;
}

LPCSTR GetError( EErrorCodes error_code )
{
	switch( error_code )
	{
		case EErrorCodes::NoError: return "No error";
		case EErrorCodes::NoLicense: return "No license was found, or the license is not valid. Please install a valid license.";
		case EErrorCodes::NotInitialized: return "The SDK is not initialized, or no process object is loaded.";
		case EErrorCodes::AlreadyInitialized: return "The SDK is already initialized. Please call Deinitialize() before calling Initialize() again.";
		case EErrorCodes::DLLOrDependenciesNotFound:
			return "The specified file was not found. This might also mean that a .dll cannot be loaded because there is a missing dependency, such as the "
			       "runtime environment.";
		case EErrorCodes::InvalidInputParam: return "An invalid parameter was passed to the method";
		case EErrorCodes::DLLAndHeaderFileIsNotMatching: return "The Simplygon DLL and header file interface versions do not match";
		case EErrorCodes::DLLFailedToLoad: return "the Simplygon DLL failed loading, probably because of a missing dependency";
		case EErrorCodes::LicenseNotForThisApplication: return "the license is not made for this application of Simplygon";
		case EErrorCodes::LicenseNotForThisPlatform: return "the license is not made for this platform of Simplygon";
		case EErrorCodes::LicenseNotForThisVersion: return "the license is not made for this version of Simplygon";
		case EErrorCodes::FailedTesting: return "the reduction failed post-testing";
		case EErrorCodes::FailedToCreatePipelineSerializer: return "failed to create pipeline serializer";
		case EErrorCodes::FailedToRunPipeline: return "failed to run pipeline";
		case (EErrorCodes)-14/*EErrorCodes::UnhandledException*/: return "unhandled exception";
		case EErrorCodes::FailedToUploadFreeLicenseAsset: return "failed to upload free license asset";
		case EErrorCodes::FailedToResolveLicenseServerHostName: return "cannot reach the licensing server, cant look up server, check DNS";
		case EErrorCodes::FailedToConnectToLicenseServer: return "cannot contact the licensing server, check firewall/proxy server";
		case EErrorCodes::FailedToConnectToLicenseServerAndSendData:
			return "Failed to connect to the license server. Please check that your internet connection is working and outgoing connections to "
			       "license.simplygon.com port 443 are not blocked by a firewall or proxy.";
		case EErrorCodes::FailedToConnectToLicenseServerAndReceiveData: return "cannot receive data from the licensing server, check firewall/proxy server";
		case EErrorCodes::DataFromLicenseServerIsCorrupt: return "data from licensing server is corrupted, try again, check connection";
		case EErrorCodes::LicenseHasExpired: return "the license has expired";
		case EErrorCodes::LicenseHashDataCorrupt: return "the license data is corrupted, please reinstall the license key";
		case EErrorCodes::LicenseNotForThisProduct: return "the license is not for this product, please contact licensing, and replace license key";
		case EErrorCodes::NoNetworkCardFound: return "no network card was found on the machine";
		case EErrorCodes::YourLicenseRequiresLatestWindows10: return "your license requires latest Windows 10 version";
		case EErrorCodes::LicenseServerReturnedBadRequest: return "Bad license request. Please reach out to SimplygonSupport[at]microsoft.com for support";
		case EErrorCodes::UnknownLicenseKey:
			return "You are using an unknown license key. Please verify so you typed in the license key correctly and try again";
		case EErrorCodes::LicenseIsNotActive: return "You are using an inactive license key. Please reach out to SimplygonSales[at]microsoft.com for support";
		case EErrorCodes::MaxNodesReachForThisLicense:
			return "You have reached the maximum number of nodes for your license. After 48 hours of not using Simplygon on a machine the license will be "
			       "released and can be installed on another machine";
		case EErrorCodes::YourLicenseRequiresLatestSimplygon:
			return "The license key you are using requires you to update to the latest version before you can use Simplygon";
		case EErrorCodes::LicenseServerReturnServerError: return "Bad license request. Please reach out to SimplygonSupport[at]microsoft.com for support";
		default: return "Extended Internal Error Code. Contact support@simplygon.com for help";
	}
}

int PollLog( char* dest, int max_len_dest )
{
	if( dest == nullptr || max_len_dest == 0 || PollLogSimplygonPtr == nullptr )
		return 0;

	auto sz = PollLogSimplygonPtr( dest, max_len_dest );
	return sz;
}

bool IsLicenseNodelocked()
{
	return LicenseIsNodelocked;
}

// end of namespace
}

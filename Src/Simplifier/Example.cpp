///////////////////////////////////////////////////////////////////////////
//
//  System:    Simplygon
//  File:      Example.cpp
//  Language:  C++
// 
//  Copyright (c) 2015 Donya Labs AB. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////
//
//	#Description# Example.h & Example.cpp implements common example 
//	functions, such as init/deinit and error handling, which are used by 
//	all examples in the Simplygon SDK.
//
///////////////////////////////////////////////////////////////////////////

#include "Simplifier/Example.h"
#include <fstream>
#include <streambuf>
#include <string>
#include <time.h>


static time_t start_time;
static time_t end_time;
static time_t delta_time;

// Error handling class
class error_handler : public Simplygon::ErrorHandler
{
public:
	virtual	void HandleError(Simplygon::spObject object, const char* interfaceName, const char* methodName, Simplygon::rid errorType, const char* errorText) override
	{
		printf("An error occured!\n");
		printf("\tInterface: %s (%p)\n", interfaceName, object);
		printf("\tMethod: %s\n", methodName);
		printf("\tError Type: %d\n", errorType);
		printf("\tError Description: %s\n", errorText);

		fprintf(stderr, "A SimplygonSDK error occured!\n");
		fprintf(stderr, "\tInterface: %s (%p)\n", interfaceName, object);
		fprintf(stderr, "\tMethod: %s\n", methodName);
		fprintf(stderr, "\tError Type: %d\n", errorType);
		fprintf(stderr, "\tError Description: %s\n", errorText);

		exit(1);
	}
}eh;

void ExitWithError(const std::string& errorstr)
{
	fprintf(stderr, "Runtime error: %s\n", errorstr.c_str());
	exit(1);
}

static int ExampleRuntimeFailure(int reportType, char* message, int* returnValue)
{
	fprintf(stderr, message);
	exit(-1);
}

// void InitExample()
// {
// 	start_time = time(nullptr);
// 
// 	// Initiate
// #ifdef _WIN32
// 	// Avoid message boxes on remote test/example execution
// 	// To get back message boxes on abort() and runtime failures, comment out these lines
// 	_set_abort_behavior(0, _CALL_REPORTFAULT);
// 	_set_error_mode(_OUT_TO_STDERR);
// 	_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, ExampleRuntimeFailure);
// 
// // 	Simplygon::AddSearchPath("..\\..\\RemeshANDRetexture\\dll\\");
// // 	Simplygon::AddSearchPath("..\\RemeshANDRetexture\\dll\\");
// 
// 	Simplygon::AddSearchPath("simplygon9_dll\\");
// 	Simplygon::EErrorCodes initval = Simplygon::Initialize(&sg, nullptr, nullptr);
// #elif defined(__linux__)
// 	std::ifstream t("LicensePathHere");
// 	std::string licenceStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
// 	int initval = SimplygonSDK::Initialize(&sg, "./SimplygonSDKNixRuntime.so", licenceStr.c_str());
// #elif defined(__APPLE__)
// 	std::ifstream t("LicensePathHere");
// 	std::string licenceStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
// 	int initval = SimplygonSDK::Initialize(&sg, "./SimplygonSDKNixRuntime.dylib", licenceStr.c_str());
// #endif
// 
// 	if (initval != Simplygon::EErrorCodes::NoError)
// 	{
// 		fprintf(stderr, "Failed to initialize.\n\tError: %d\n\tError string:%s\n", initval, Simplygon::GetError(initval));
// 		exit(1);
// 	}
// 	sg->SetErrorHandler(&eh);
// 
// 	// Set working directory to executable directory
// #ifdef _WIN32
// 	SetCurrentDirectory(GetExecutablePath().c_str());
// #elif defined(__linux__) || defined(__APPLE__)
// 	chdir(GetExecutablePath().c_str());
// #endif
// }

// void DeinitExample()
// {
// 	// Deinitialize the SDK
// 	Simplygon::Deinitialize(sg);
// 
// 	end_time = time(nullptr);
// 	delta_time = end_time - start_time;
// 
// 	printf("\nProcessing took %lld seconds.\n", delta_time);
// }

std::string GetExecutablePath()
{
#ifdef _WIN32

	char currentDir[MAX_PATH];
	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char filename[MAX_PATH];
	char ext[MAX_PATH];

	if (GetModuleFileName(NULL, currentDir, MAX_PATH) == NULL)
	{
		return std::string("");
	}

	_splitpath_s(currentDir, drive, _MAX_DRIVE, dir, _MAX_DIR, filename, _MAX_FNAME, ext, _MAX_EXT);
	_makepath_s(currentDir, _MAX_PATH, drive, dir, "", "");

	return std::string(currentDir);

#elif defined(__linux__)

	char currentDir[MAX_PATH];
	ssize_t len = readlink("/proc/self/exe", currentDir, MAX_PATH);
	currentDir[len] = '\0';

	std::string path = currentDir;
	path = path.substr(0, path.find_last_of("/\\")) + "/";
	return path;

#elif defined(__APPLE__)

	char currentDir[MAX_PATH];
	uint32_t bufsize = MAX_PATH;
	if (_NSGetExecutablePath(currentDir, &bufsize) == -1)
	{
		return std::string("");
	}
	return std::string(currentDir);

#endif
}

std::string GetAssetPath()
{
	std::string assetPath;
	std::string currentWorkingDirectory = GetExecutablePath();
	std::vector<std::string> folders = StringSplit(currentWorkingDirectory.c_str(), _T("\\/"));

	// Look for Build folder, Asset folder should be next to it
	std::vector<std::string>::iterator it;
	for (it = folders.begin(); it < folders.end(); it++)
	{
		if (*it == _T("Build"))
		{
			break;
		}
		assetPath += *it;
		assetPath += "/";
	}

	assetPath += _T("Assets/");
#if defined(__linux__) || defined(__APPLE__)
	assetPath = "/" + assetPath;
#endif
	return assetPath;
}


std::vector<std::string> StringSplit(const std::string& source, const std::string& delim)
{

	std::stringstream stringStream(source);
	std::string line;
	std::vector<std::string> stringCollection;

	while (std::getline(stringStream, line))
	{
		std::size_t prev = 0;
		std::size_t pos;
		while ((pos = line.find_first_of(delim, prev)) != std::string::npos)
		{
			if (pos > prev)
			{
				stringCollection.push_back(line.substr(prev, pos - prev));
			}
			prev = pos + 1;
		}
		if (prev < line.length())
		{
			stringCollection.push_back(line.substr(prev, std::string::npos));
		}
	}
	return stringCollection;
}

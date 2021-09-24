///////////////////////////////////////////////////////////////////////////
//
//  System:    Simplygon
//  File:      Example.h
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

#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

// standard headers
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <float.h>
#include "Matrix4x4.h"

#ifdef _WIN32

#include <Simplifier/SimplygonLoader.h>
#include <windows.h>
#include <process.h>

#elif defined(__linux__) || defined(__APPLE__)

#include <SimplygonNixLoader.h>
#include <pthread.h>
#include <unistd.h>

#ifndef MAX_PATH
#if defined(__linux__)
#include <linux/limits.h>
#elif defined(__APPLE__)
#include <sys/syslimits.h>
#endif
#define MAX_PATH PATH_MAX
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#endif
// use SimplygonSDK as a default namespace
using namespace Simplygon;

// all examples expect a global variable sg that has
// a pointer to the simplygon interface
//extern ISimplygon* sg;

// main example init/deinit function
void InitExample();
void DeinitExample();
std::string GetExecutablePath();
std::string GetAssetPath();

std::vector<std::string> StringSplit(const std::string& source, const std::string& splitOn);

//void AddSimplygonTexture(spMaterial mat, spTextureTable texTable, const char* channelName, const char* texFilePath, const char* namePrefix = "");

void ExitWithError(const std::string& errorstr);

inline REMTNS::Matrix4x4 GetMatrix4x4FromIMatrix(spMatrix4x4 src)
{
	//<add tag=1725 date=2021-03-30>
	Simplygon::spRealData srcElements = src->GetElements();
	//</add>

	//real srcElements[16];
	//src->GetElements(srcElements);

	return REMTNS::Matrix4x4(srcElements);
}

inline real GetRadFromDegrees(real angle)
{
	return (real)(3.141592 / 180) * angle;
}


static inline void PrintProgressBar(int current)
{
	int width = 40;

	//Start
	printf("\r");
	printf("[");

	float ratio = (float)current / 100.f;
	int c = int(ratio * (float)width);

	//Filled
	for (int x = 0; x < c && x < width; x++)
		printf("=");

	//Not filled
	for (int x = c; x < width; x++)
		printf(" ");

	//End, percent
	printf("] %3d%%", (int)(ratio * 100));

	fflush(stdout);
}

#endif //__EXAMPLE_H__

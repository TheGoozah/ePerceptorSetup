// NativeETDll.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"

#include "NativeETDll.h"
#include "..\NativeETLib\ETHost.h"
#include "..\NativeETLib\include\tobiigaze_data_types.h"

#include <vector>
using namespace std;

//http://stackoverflow.com/questions/75701/what-happens-to-global-variables-declared-in-a-dll

//CORE FUNCTIONS
extern "C" NATIVEDLL_API void __cdecl BootETHost()
{
	ETHOST->BootETHost(); //Creates an instance (first call) and boots the ETHost
}

extern "C" NATIVEDLL_API void __cdecl UpdateETHost()
{
	ETHOST->UpdateETHost(); //Update the ETHost and all it's managers
}

extern "C" NATIVEDLL_API void __cdecl DeleteSingleton()
{
	if (ETHOST != nullptr)
		delete ETHOST;
}

//DATA STREAMING FUNCTIONS
extern "C" NATIVEDLL_API void* __cdecl GetAllData(int* pSize)
{
	if (ETHOST == nullptr)
		return nullptr;

	void* ptr = ETHOST->GetAllData(pSize);
	//printf("Address Method2 return via Native Dynamic DLL: %#x \n", ptr);
	return ptr;
}

extern "C" NATIVEDLL_API void* __cdecl GetDataStream(int* pSize)
{
	if (ETHOST == nullptr)
		return nullptr;

	void* ptr = ETHOST->GetDataStream(pSize);
	return ptr;
}

extern "C" NATIVEDLL_API void __cdecl Flush()
{
	ETHOST->Flush();
}

//CALIBRATION FUNCTIONS
extern "C" NATIVEDLL_API void __cdecl InvokeCalibration()
{
	ETHOST->InvokeCalibration();
}

extern "C" NATIVEDLL_API bool __cdecl GetCalibrationPointData(double* pX, double* pY, double* pScale)
{
	if (ETHOST == nullptr)
		return false;

	return ETHOST->GetCalibrationPointData(pX, pY, pScale);
}

extern "C" NATIVEDLL_API bool __cdecl IsCalibrated()
{
	return ETHOST->IsCalibrated();
}
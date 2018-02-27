#pragma once

//Function treatment defines
#ifdef NATIVEETDLL_EXPORTS
#define NATIVEDLL_API __declspec(dllexport)
#else
#define NATIVEDLL_API __declspec(dllimport)
#endif

#include <vector>
#include "../NativeETLib/include/tobiigaze_data_types.h"

//Defined DLL functions
//CORE FUNCTONS
extern "C" NATIVEDLL_API void __cdecl BootETHost();
extern "C" NATIVEDLL_API void __cdecl UpdateETHost();
extern "C" NATIVEDLL_API void __cdecl DeleteSingleton();

//DATA STREAMING FUNCTIONS
extern "C" NATIVEDLL_API void* __cdecl GetAllData(int* size);
extern "C" NATIVEDLL_API void* __cdecl GetDataStream(int* size);
extern "C" NATIVEDLL_API void __cdecl Flush();

//CALIBRATION
extern "C" NATIVEDLL_API void __cdecl InvokeCalibration();
extern "C" NATIVEDLL_API bool __cdecl GetCalibrationPointData(double* X, double* Y, double* Scale);
extern "C" NATIVEDLL_API bool __cdecl IsCalibrated();
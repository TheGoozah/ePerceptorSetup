#pragma once

#include "stdafx.h"
#include "ETDebug.h"
#include "include/tobiigaze.h"

#ifdef _WIN32
#include <Windows.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#endif


//Constructor
ETDebug::ETDebug()
{
	CreateConsoleWindow();
}

//Destructor
ETDebug::~ETDebug()
{
	DestroyConsoleWindow();
}

//Public Functions
void ETDebug::ReportError(tobiigaze_error_code error_code, const char* error_message)
{
	if (error_code)
	{
		fprintf(stderr, "Error: %d(%s). \n", error_code, error_message);
		fprintf(stderr, "Error means: %d(%s). \n", error_code, error_message);
		fprintf(stderr, "Error: %s\n", tobiigaze_get_error_message(error_code));
	}
}

//Private Functins
void ETDebug::CreateConsoleWindow()
{
	//If windows... (_WIN32: Defined for applications for Win32 and Win64)
#ifdef _WIN32
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
#endif
}

void ETDebug::DestroyConsoleWindow()
{
#ifdef _WIN32
	FreeConsole();
#endif
}
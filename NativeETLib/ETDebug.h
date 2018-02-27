#pragma once

#include "include/tobiigaze_error_codes.h"

//ref: http://www.cplusplus.com/reference/cstdio/printf/

class ETDebug final
{
public:
	//Constructor & Destructor
	ETDebug();
	~ETDebug(); //Non-virtual because this is a final class that does not inherite from any class

	//Functions
	void ReportError(tobiigaze_error_code error_code, const char* error_message);

private:
	//Functions
	void CreateConsoleWindow();
	void DestroyConsoleWindow();
};
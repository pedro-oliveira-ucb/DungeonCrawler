#pragma once
#include <iostream>

class Log
{
public:
	static void Print( const char * format , ... );
	static void Error( const char * format , va_list args );
};


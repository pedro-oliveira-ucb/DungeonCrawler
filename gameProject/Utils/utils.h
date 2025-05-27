#pragma once
#include "singleton.h"
#include <string>

class utils : public CSingleton<utils>
{
public:
	int randomNumber( int min , int max ) const;
	std::string readFileAsString( const std::string & filename ) const;

};


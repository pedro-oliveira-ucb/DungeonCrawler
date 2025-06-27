#pragma once
#include "singleton.h"
#include <string>

class utils : public CSingleton<utils>
{
	
public:
	utils( );
	bool onProbability( int probability ) const;
	int randomNumber( int min , int max ) const;
	std::string readFileAsString( const std::string & filename ) const;
	std::pair<int , int> getScreenSize( ) const;
	void freeConsole( );
};


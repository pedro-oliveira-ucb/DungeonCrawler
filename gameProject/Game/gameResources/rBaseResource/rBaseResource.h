#pragma once
#include <string>

enum rBaseResourceType {
	SPRITE,
	IMAGE,
	VIDEO
};

class rBaseResource 
{
	std::string Path;
public:
	rBaseResource( std::string path );
	std::string getPath( );
};


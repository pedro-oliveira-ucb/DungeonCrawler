#pragma once
#include <string>
#include <vector>

#include "../../rBaseResource/rBaseResource.h"

class rSoundsManager : public rBaseResource
{ 
public:
	rSoundsManager( std::string path ) :rBaseResource( path ) { }

	bool initialize( ) override;

};


#pragma once
#include <string>
#include "../../../../Utils/singleton.h"

class TrapsInitializer : public CSingleton< TrapsInitializer>
{
public:
	bool initialize( );
};


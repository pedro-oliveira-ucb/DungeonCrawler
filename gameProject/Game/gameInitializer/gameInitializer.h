#pragma once
#include "../../../Utils/singleton.h"

class gameInitializer : public CSingleton<gameInitializer>
{
public:
	bool initializeResources( );
};


#pragma once
#include "../../Utils/singleton.h"

class gameRender : public CSingleton<gameRender>
{
public:
	void render( );
};


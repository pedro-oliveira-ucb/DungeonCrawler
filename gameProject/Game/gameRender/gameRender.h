#pragma once
#include "../../Utils/singleton.h"

class gameRender : public CSingleton<gameRender>
{
	void renderCustomCursor( );

public:
	void render( );
	void soundEvents( );
};
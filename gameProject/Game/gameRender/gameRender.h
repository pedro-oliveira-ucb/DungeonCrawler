#pragma once
#include "../../Utils/singleton.h"




class gameRender : public CSingleton<gameRender>
{
	float zoomLevel = 1.0f;

	void renderCustomCursor( );
	void renderDialogs( );
	void correctMousePosition( );

public:
	void render( );
	void processSoundEvents( );
};
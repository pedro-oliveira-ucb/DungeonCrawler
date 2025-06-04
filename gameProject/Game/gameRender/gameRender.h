#pragma once
#include "../../Utils/singleton.h"

class gameRender : public CSingleton<gameRender>
{
	float zoomLevel = 1.0f;

public:
	void render( );
	void renderCustomCursor( );
	void processSoundEvents( );
	void renderDialogs( );
	void renderPauseMenu( );
	void renderEntities( );
	void renderGameOver( );
	void renderMap( );
	void renderMapDoors( );
};
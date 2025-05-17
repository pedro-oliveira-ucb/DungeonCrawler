#pragma once
#include "../CBaseThread/CBaseThread.h"
#include "../../../SDK/math/Vector2D/GVector2D.h"


class Render : public CBaseThread
{
	GVector2D windowSize;

public:
	void threadFunction( ) override {

	}

	bool InitGameWindow( );
};

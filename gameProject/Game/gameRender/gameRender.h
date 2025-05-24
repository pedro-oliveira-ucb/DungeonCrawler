#pragma once
#include "../../Utils/singleton.h"
#include <SFML/Graphics.hpp>

class gameRender : public CSingleton<gameRender>
{
	void renderCustomCursor( );

public:
	void render( sf::RenderWindow & window );
	void soundEvents( );
};


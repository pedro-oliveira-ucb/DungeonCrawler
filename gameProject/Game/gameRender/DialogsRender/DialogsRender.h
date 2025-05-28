#pragma once
#include <string>
#include "../../gameObjects/gamdDialogHandler/gameDialogHandler.h"
#include <raylib/raylib.h>

struct dialogBox {
	std::string text;
	Font textFont;
	float textWidth;
	float textHeight;

	float padding; // padding around the text
	int fontSize;
	float x , y , width , height;
};

class DialogsRender 
{
	float dialogStartTime;
	float dialogEndTime;
	float currentDialogTime;
	gameDialog currentDialog;
	gameDialog tempDialog;
	dialogBox renderDialogBox( std::string, Font);
	void renderDialogText( dialogBox );
public:
	void render( );
};


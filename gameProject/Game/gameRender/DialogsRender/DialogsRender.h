#pragma once
#include <string>
#include "../../Handlers/gamdDialogHandler/gameDialogHandler.h"
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
	float dialogStartTime = 0;
	float dialogEndTime = 0;
	float currentDialogTime = 0;
	gameDialog currentDialog;
	gameDialog tempDialog;
	dialogBox renderDialogBox( std::string, Font);
	void renderDialogText( dialogBox );
public:
	void render( );
};


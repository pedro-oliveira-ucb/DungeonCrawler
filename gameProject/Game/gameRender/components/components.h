#pragma once
#include "../../../Utils/singleton.h"

#include <raylib/raylib.h>


class components : public CSingleton<components>
{
public:
	
	bool DrawSlider( Rectangle bounds , float * value , const char * label , float min , float max , Color backColor , Color frontColor );
	bool DrawButton( Rectangle bounds , const char * label , Color color = LIGHTGRAY , Color textColor = DARKGRAY, Color borderColor = LIGHTGRAY );
	bool DrawTextButton( Vector2 position , const char * text , int fontSize, Color color = LIGHTGRAY , Color hoveredColor = DARKGRAY );
};


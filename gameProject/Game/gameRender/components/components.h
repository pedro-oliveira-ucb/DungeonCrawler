#pragma once
#include "../../../Utils/singleton.h"

#include <raylib/raylib.h>


class components : public CSingleton<components>
{
public:
	
	bool DrawSlider( Rectangle bounds , float * value , const char * label , float min , float max , Color backColor , Color frontColor );
	bool DrawButton( Rectangle bounds , const char * label , Color color = LIGHTGRAY , Color textColor = DARKGRAY );
};


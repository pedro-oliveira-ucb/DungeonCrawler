#include "renderCustomCursor.h"

#include "../../../Globals/Globals.h"

#include <raylib/raylib.h>

void renderCustomCursor::render( ) {


	DrawCircle( Globals::Get( ).mousePosX , Globals::Get( ).mousePosY , 10 , BLUE );
}

#include "mouseMovement.h"
#include <Windows.h>
#include <cmath>

#define M_PI  3.14159265358979323846

#include "../../SDK/math/gAngle/GAngle.h"
#include "../../../Globals/Globals.h"

float mouseMovement::getMouseAngle( float x, float y) {


    POINT cursorPos;
    GetCursorPos( &cursorPos );

    globals.mousePosX = cursorPos.x;
    globals.mousePosY = cursorPos.y;

    float dx = static_cast< float >( cursorPos.x - x );
    float dy = static_cast< float >( cursorPos.y - y );
    float length = std::sqrt( dx * dx + dy * dy );

    float angleInRadians = 0.0f;

    if ( length != 0 ) {
        angleInRadians = std::atan2( dy , dx );
    }

    // Converte para graus antes de retornar
    return GAngle::toDegrees( angleInRadians );
}
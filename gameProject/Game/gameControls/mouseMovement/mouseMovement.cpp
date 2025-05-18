#include "mouseMovement.h"
#include <Windows.h>
#include <cmath>

#define M_PI  3.14159265358979323846

#include "../../../Globals/Globals.h"

mouseAngle mouseMovement::getMouseAngle( ) {
	mouseAngle mAngle;

	int centerX = globals.screenWidth / 2;
	int centerY = globals.screenHeight / 2;

	POINT cursorPos;
	GetCursorPos( &cursorPos );
	float dx = static_cast< float >( cursorPos.x - centerX );
	float dy = static_cast< float >( cursorPos.y - centerY );
	float length = std::sqrt( dx * dx + dy * dy );


	if ( length == 0 ) {
		mAngle.cos = 1.0f; // mouse no centro, cosseno = 1 (ângulo 0)
		mAngle.sin = 0.0f; // mouse no centro, seno = 0 (ângulo 0)
	}
	else {
		mAngle.cos = dx / length;
		mAngle.sin = dy / length;
	}

	return mAngle;
}
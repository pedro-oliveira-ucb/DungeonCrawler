#include "math.h"

#include <cmath>

int math::sign( float x ) {
	return ( x > 0 ) - ( x < 0 );
}
float math::AngleDiff( float a , float b ) {
	float diff = fmodf( a - b + 180.0f , 360.0f );
	if ( diff < 0 ) diff += 360.0f;
	return diff - 180.0f;
}
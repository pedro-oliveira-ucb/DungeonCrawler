#pragma once
#include "../../../Utils/singleton.h"


class mouseMovement : public CSingleton<mouseMovement>
{
public:
    float getMouseAngle( float x, float y ); // cosseno do ângulo

};

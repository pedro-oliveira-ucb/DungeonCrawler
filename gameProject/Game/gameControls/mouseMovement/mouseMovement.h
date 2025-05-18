#pragma once
#include "../../../Utils/singleton.h"

struct mouseAngle {
    float cos;
    float sin;
};

class mouseMovement : public CSingleton<mouseMovement>
{
public:
    mouseAngle getMouseAngle( ); // cosseno do ângulo

};

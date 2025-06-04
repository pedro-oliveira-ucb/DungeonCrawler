#pragma once
#include <vector>
#include "../../SDK/math/Vector2D/GVector2D.h"

struct DoorMapObject {
	GVector2D position;
	bool unlocked = false;
};

class renderMap
{
	std::vector<DoorMapObject> doors;
public:
	void render( );
	void renderDoors( );
};


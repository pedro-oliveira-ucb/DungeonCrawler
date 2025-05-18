#pragma once
#include "../SDK/Entities/CPlayerEntity/CPlayerEntity.h"

class gameWorld 
{
public:
	CPlayerEntity * localplayer = nullptr;
	std::vector<CBaseEntity *> entities;
};
extern gameWorld _gameWorld;

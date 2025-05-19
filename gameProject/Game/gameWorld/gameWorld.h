#pragma once
#include "../SDK/Entities/CPlayerEntity/CPlayerEntity.h"
#include "../SDK/Entities/Attacks/CBaseAttack/CBaseAttack.h"

class gameWorld 
{
public:
	CPlayerEntity * localplayer = nullptr;
	std::vector<CBaseEntity *> availableEntities;
	std::vector<CBaseAttack *> availableAttacks;
};
extern gameWorld _gameWorld;

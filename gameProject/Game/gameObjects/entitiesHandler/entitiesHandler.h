#pragma once
#include "../../../Utils/singleton.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"


class entitiesHandler : public CSingleton<entitiesHandler>
{
	CPlayerEntity * localPlayer;

public:
	void setLocalPlayer( CPlayerEntity * address );
	CPlayerEntity * getLocalPlayer( );
	
};


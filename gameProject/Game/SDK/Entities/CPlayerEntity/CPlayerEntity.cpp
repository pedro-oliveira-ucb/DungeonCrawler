#include "CPlayerEntity.h"

#include <mutex>

std::mutex localPlayerMutex;

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder )
	: CBaseEntity( builder ) 
{

}



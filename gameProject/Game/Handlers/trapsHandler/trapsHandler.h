#pragma once

#include "../../../Utils/singleton.h"
#include "../../SDK/Entities/Traps/CBaseTrap/CBaseTrap.h"

class trapsHandler : public CSingleton<trapsHandler>
{
	std::mutex handlerMutex;
	std::unordered_map<TrapType , std::unique_ptr<CBaseTrap>> spawnableTraps;
	std::vector<std::unique_ptr<CBaseTrap>> spawnedTraps;

public:
	void updateTraps( );

	void addSpawnableTrap( std::unique_ptr<CBaseTrap> item );
	std::unique_ptr<CBaseTrap> * getSpawnableTrap( TrapType type );

	CBaseTrap * spawnTrap ( TrapType type, GVector2D position);
	void clearSpawnedTraps( );

	std::vector<std::unique_ptr<CBaseTrap>> * getSpawnedTraps( );
};


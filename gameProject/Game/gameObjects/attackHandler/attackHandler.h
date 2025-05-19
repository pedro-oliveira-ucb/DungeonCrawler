#pragma once
#include <vector>
#include <memory>
#include "../../../Utils/singleton.h"

#include "../../SDK/Entities/Attacks/CBaseAttack/CBaseAttack.h"


class attackHandler : public CSingleton<attackHandler>
{
	std::vector<std::pair<CBaseEntity * ,std::shared_ptr<CBaseAttack>>> runningAttacks;

	std::vector<std::shared_ptr<CBaseAttack>> availableEnemiesAttacks;
	std::vector<std::shared_ptr<CBaseAttack>> availableLocalPlayerAttacks;

public:

	int runningAttacksSize( );
	std::shared_ptr<CBaseAttack> getRunningAttack( int index );

	std::shared_ptr<CBaseAttack> getAvailableLocalPlayerAttack( int index );
	std::shared_ptr<CBaseAttack> getAvailableEnemyAttack( int index );

	void addAvailableLocalPlayerAttack( std::shared_ptr<CBaseAttack> attack );
	void addAvailableEnemyAttack( std::shared_ptr<CBaseAttack> attack );

	void updateAttacks( );
	std::shared_ptr<CBaseAttack> throwNewAttack( CBaseEntity * sender , CBaseAttack * attack );
};


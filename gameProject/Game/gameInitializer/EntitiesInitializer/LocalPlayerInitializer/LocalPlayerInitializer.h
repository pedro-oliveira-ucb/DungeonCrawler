#pragma once
#include "../../../Utils/singleton.h"
#include "../../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"

class LocalPlayerInitializer : public CSingleton<LocalPlayerInitializer>
{
public:
	CPlayerEntity * generate( std::string animationName );
};


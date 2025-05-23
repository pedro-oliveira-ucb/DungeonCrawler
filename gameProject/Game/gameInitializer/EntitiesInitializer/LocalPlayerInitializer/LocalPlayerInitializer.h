#pragma once
#include "../../../Utils/singleton.h"
#include "../../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"

class LocalPlayerInitializer : public CSingleton<LocalPlayerInitializer>
{
	CPlayerEntity * generate( std::string animationName );

	bool initializeEvents( );

public:
	bool initialize( );
};


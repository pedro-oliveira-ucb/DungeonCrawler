#pragma once
#include "../CBaseThread/CBaseThread.h"

class updateEntities : public CBaseThread
{
public:
	void threadFunction( ) override;
};


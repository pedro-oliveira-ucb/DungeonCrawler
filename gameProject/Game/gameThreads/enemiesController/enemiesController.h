#pragma once
#include "../CBaseThread/CBaseThread.h"

class enemiesController : public CBaseThread
{
public:
	void threadFunction( ) override;
};


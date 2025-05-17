#pragma once
#include "../CBaseThread/CBaseThread.h"

class eventHandlers : public CBaseThread
{
public:
	void threadFunction( ) override;
};


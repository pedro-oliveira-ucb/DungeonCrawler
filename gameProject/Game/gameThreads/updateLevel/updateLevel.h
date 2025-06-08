#pragma once
#include "../CBaseThread/CBaseThread.h"

class updateLevel : public CBaseThread {


public:
	void threadFunction( ) override;
};
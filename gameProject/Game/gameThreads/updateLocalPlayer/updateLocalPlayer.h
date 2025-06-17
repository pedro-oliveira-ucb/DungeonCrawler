#pragma once
#include "../CBaseThread/CBaseThread.h"



class updateLocalPlayer : public CBaseThread {


public:
	void threadFunction( ) override;
};
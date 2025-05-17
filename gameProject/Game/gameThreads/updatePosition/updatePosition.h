#pragma once
#include "../CBaseThread/CBaseThread.h"



class updatePositionEvent : public CBaseThread {


public:
	void threadFunction( ) override;
};
#pragma once
#include "../../../Utils/singleton.h"

#include <string>
#include <mutex>
#include <queue>

class gameSoundsEventHandler : public CSingleton<gameSoundsEventHandler>
{
	std::mutex threadLock;
	std::queue<std::string> soundQueue;

public:
	void addEventToQueue( std::string );
	std::string getLatestOnQueue( );
};

#pragma once
#include <string>
#include <mutex>
#include <queue>

class gameSoundsEventHandler
{
	std::mutex threadLock;
	std::queue<std::string> soundQueue;

public:
	void addEventToQueue( std::string );
	std::string getLatestOnQueue( );
};

extern gameSoundsEventHandler gameSoundsQueue;
#pragma once
#include "../../../Utils/singleton.h"

#include <string>
#include <mutex>
#include <queue>

enum soundEventType {
	PLAY,
	STOP
};

struct soundEvent {
	soundEventType type;
	std::string soundName;

	soundEvent( std::string name , soundEventType t = PLAY ) : soundName( name ) , type( t ) { }
};


class gameSoundsEventHandler : public CSingleton<gameSoundsEventHandler>
{
	std::mutex threadLock;
	std::queue<soundEvent> soundQueue;

public:
	void addEventToQueue( std::string, soundEventType type = PLAY );
	soundEvent getLatestOnQueue( );
};

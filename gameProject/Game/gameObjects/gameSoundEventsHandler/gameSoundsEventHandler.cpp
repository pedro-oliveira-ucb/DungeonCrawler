#include "gameSoundsEventHandler.h"

gameSoundsEventHandler gameSoundsQueue;

void gameSoundsEventHandler::addEventToQueue( std::string eventName) {
	std::lock_guard<std::mutex> lock( this->threadLock );
	this->soundQueue.emplace( eventName );
}

std::string gameSoundsEventHandler::getLatestOnQueue( )
{
	std::lock_guard<std::mutex> lock( this->threadLock );
	if ( this->soundQueue.empty( ) ) 
		return "";

	std::string eventName = this->soundQueue.front( );
	this->soundQueue.pop( );
	return eventName;
}


#include "gameSoundsEventHandler.h"

gameSoundsEventHandler gameSoundsQueue;

void gameSoundsEventHandler::addEventToQueue( std::string eventName , soundEventType type ) {
	std::lock_guard<std::mutex> lock( this->threadLock );
	this->soundQueue.emplace( soundEvent( eventName , type ) );
}

soundEvent gameSoundsEventHandler::getLatestOnQueue( )
{
	soundEvent event( "" , PLAY );

	std::lock_guard<std::mutex> lock( this->threadLock );
	if ( this->soundQueue.empty( ) )
		return event;

	soundEvent latestEvent = this->soundQueue.front( );
	this->soundQueue.pop( );
	return latestEvent;

}

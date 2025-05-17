#include "gameThreads.h"
#include <thread>
#include <vector>
#include <string>

#include "eventHandlers/eventHandlers.h"
#include "updatePosition/updatePosition.h"
#include "../../Utils/Log/Log.h"


updatePositionEvent _updatePositionEvent;

std::vector<std::pair<CBaseThread *, std::string> > _gameThreads {
	{  &_updatePositionEvent, "updatePosition" }
	//{  &gameRender, "gameRender" }
};

bool gameThreads::deploy( ) {

	for ( int i = 0; i < _gameThreads.size( ); i++ ) {
		std::pair<CBaseThread * , std::string> thread = _gameThreads.at( i );
		Log::Print( "[gameThreads] Initializing %s!" , thread.second );
		thread.first->start( );
		Log::Print( "[gameThreads] %s initialized!" , thread.second );
	}

	return true;
}
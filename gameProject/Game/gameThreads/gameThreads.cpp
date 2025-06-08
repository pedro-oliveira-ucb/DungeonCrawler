#include "gameThreads.h"
#include <thread>
#include <vector>
#include <string>

#include "updatePosition/updatePosition.h"
#include "updateEntities/updateEntities.h"
#include "enemiesController/enemiesController.h"
#include "updateLevel/updateLevel.h"
#include "../../Utils/Log/Log.h"


updatePositionEvent _updatePositionEvent;
updateEntities _updateEntities;
enemiesController _enemiesController;
updateLevel _updateLevel;

std::vector<std::pair<CBaseThread * , std::string> > _gameThreads {
	{  &_updatePositionEvent, "updatePosition" },
	{ &_updateEntities, "updateEntities" },
	{ &_enemiesController, "enemiesController" },
	{ &_updateLevel, "updateLevel" }
};

bool gameThreads::deploy( ) {

	for ( int i = 0; i < _gameThreads.size( ); i++ ) {
		std::pair<CBaseThread * , std::string> thread = _gameThreads.at( i );
		Log::Print( "[gameThreads] Initializing %s!" , thread.second.c_str() );
		thread.first->start( );
		Log::Print( "[gameThreads] %s initialized!" , thread.second.c_str() );
	}

	return true;
}
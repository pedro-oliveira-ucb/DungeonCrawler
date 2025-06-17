#include "gameThreads.h"
#include <thread>
#include <vector>
#include <string>

#include "updateLocalPlayer/updateLocalPlayer.h"
#include "updateEntities/updateEntities.h"
#include "updateLevel/updateLevel.h"
#include "../../Utils/Log/Log.h"


updateLocalPlayer _updateLocalPlayer;
updateEntities _updateEntities;
updateLevel _updateLevel;

std::vector<std::pair<CBaseThread * , std::string> > _gameThreads {
	{  &_updateLocalPlayer, "updateLocalPlayer" },
	{ &_updateEntities, "updateEntities" },
	{ &_updateLevel, "updateLevel" }
};

bool gameThreads::deploy( ) {

	for ( int i = 0; i < _gameThreads.size( ); i++ ) {
		std::pair<CBaseThread * , std::string> thread = _gameThreads.at( i );
		Log::Print( "[gameThreads] initializing %s!" , thread.second.c_str() );
		thread.first->start( );
		Log::Print( "[gameThreads] %s initialized!" , thread.second.c_str() );
	}

	return true;
}
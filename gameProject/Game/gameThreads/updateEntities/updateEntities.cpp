#include "updateEntities.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Managers/LevelManager/LevelManager.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/attackHandler/attackHandler.h"
#include "../../Handlers/itemsHandler/itemsHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

void updateEntities::threadFunction( ) {

	Log::Print( "[updateEntities] Hello world!" );

	while ( true ) {
		attackHandler::Get( ).updateAttacks( );
		itemsHandler::Get( ).updateItems( );
		trapsHandler::Get( ).updateTraps( );

		entitiesHandler::Get( ).updateEnemiesCollision( );

		levelManager.updateLevel( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}

	Log::Print( "[updateEntities] bye!" );
}
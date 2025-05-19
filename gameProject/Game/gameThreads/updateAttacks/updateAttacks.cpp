#include "updateAttacks.h"

#include "../../gameObjects/attackHandler/attackHandler.h"

#include "../../../Utils/Log/Log.h"



void updateAttacks::threadFunction( ) {

	Log::Print( "[updateAttacks] hello world!" );

	while ( true ) {


		attackHandler::Get( ).updateAttacks( );


		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}
#include "updateAttacks.h"


#include "../../../Utils/Log/Log.h"



void updateAttacks::threadFunction( ) {

	Log::Print( "[updateAttacks] hello world!" );

	while ( true ) {





		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}
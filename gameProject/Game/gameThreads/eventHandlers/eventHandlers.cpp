#include "eventHandlers.h"
#include "../../../Utils/Log/Log.h"


void eventHandlers::threadFunction( ) {

	while(true ){
	
	
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ));
	}
}
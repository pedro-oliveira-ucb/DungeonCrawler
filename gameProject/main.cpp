#include <iostream>
#include <thread>

#include "Game/gameThreads/gameThreads.h"
#include "Globals/Globals.h"

#include "Utils/Log/Log.h"

gameThreads gameThreads_;

//Aguarda o jogo ser encerrado
void idle( ) {
	Log::Print( "[main] Entering idle!" );
	while ( !globals.exitGame ) {
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
	Log::Print( "[main] Exiting game!" );
}

int main( ) {

	Log::Print( "[main] Initializing game threads!" );
	gameThreads_.deploy( );
	Log::Print( "[main] Game threads initialized successfully!" );


	idle( );
	return 1;
}
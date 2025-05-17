#include "gameWindow.h"

#include <thread>

#include "../../../Utils/Log/Log.h"
#include "../../../Globals/Globals.h"

#include <raylib/raylib.h>

bool InitGameWindow( ) {

	while ( !globals.exitGame ) {

		Log::Print( "[Render] Initialized window!" );

		//	raylib::SetConfigFlags( raylib::FLAG_FULLSCREEN_MODE ); // Define a flag de fullscreen
		InitWindow( 700 , 700, "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen

		globals.windowInitialized = true;

		while ( !globals.resourcesLoaded ) {
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		}

		while ( !WindowShouldClose( ) )
		{
			if ( globals.updateWindow ) {
				globals.updateWindow = false;
				break;
			}

			BeginDrawing( );
			ClearBackground( LIME );
			DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , BLACK );
			EndDrawing( );
			
		}

		CloseWindow( );
	}

	return true;
}

bool gameWindow::initialize( ) {
	std::thread( InitGameWindow ).detach( );

	while ( !globals.windowInitialized ) {
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}

	return true;
}
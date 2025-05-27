#include <iostream>
#include <thread>
#include <sstream>  // Necess�rio para std::stringstream
#include <iomanip>  // Necess�rio para std::setw e std::setfill


#include "Game/gameControls/keybindHandler/keybindHandler.h"
#include "Game/game.h"
#include "Game/gameRender/gameRender.h"
#include "Globals/Globals.h"
#include "Utils/Log/Log.h"

Game game;

#include <raylib/raylib.h>

void idle( ) {
	Log::Print( "[main] Entering idle!" );
	while ( !globals.exitGame ) {
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
	Log::Print( "[main] Exiting game!" );
}

void waitGameLoad( ) {
	while ( !game.isResourcesLoaded( ) ) {
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
}

int main( void ) {

	Log::Print( "[Render] Initialized window!" );

	SetConfigFlags( FLAG_VSYNC_HINT );

	//	raylib::SetConfigFlags( raylib::FLAG_FULLSCREEN_MODE ); // Define a flag de fullscreen
	InitWindow( globals.screenWidth , globals.screenHeight , "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen
	InitAudioDevice( );      // Initialize audio device

	DisableCursor( );

	Log::Print( "[main] starting game!" );
	game.start( );
	Log::Print( "[main] game started!" );

	waitGameLoad( );


	while ( true )
	{
		if ( globals.updateWindow ) {
			globals.updateWindow = false;
			break;
		}

		Vector2 mousePos = GetMousePosition( );
		globals.mousePosX = mousePos.x;
		globals.mousePosY = mousePos.y;

		keybindHandler::Get( ).update( );
		gameRender::Get( ).soundEvents( );

		BeginDrawing( );
		//remove old draws?
		ClearBackground( GRAY );

		gameRender::Get( ).render( );

		//DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , BLACK );
		EndDrawing( );
	}

	CloseWindow( );

	idle( );
	return 1;
}
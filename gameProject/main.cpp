#include <iostream>
#include <thread>

#include "Game/game.h"
#include "Game/gameResources/gameResourceManager/gameResourceManager.h"
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

	//	raylib::SetConfigFlags( raylib::FLAG_FULLSCREEN_MODE ); // Define a flag de fullscreen
	InitWindow( 700 , 700 , "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen

	Log::Print( "[main] starting game!" );
	game.start( );
	Log::Print( "[main] game started!" );

	waitGameLoad( );

	while ( !globals.exitGame ) {
		while ( !WindowShouldClose( ) )
		{
			if ( globals.updateWindow ) {
				globals.updateWindow = false;
				break;
			}

			auto spriteAnimations = static_cast< gameResourceManager * >( globals.resourceManagerPointer )->getSpritesManager( ).getSpriteAnimations( );
			if ( spriteAnimations->find( "Explosion_1" ) != spriteAnimations->end( ) ) {
				DrawTexture( *spriteAnimations->at( "Explosion_1" ).getCurrentTexture( ), 50, 50, WHITE );
				spriteAnimations->at( "Explosion_1" ).updateAnimation( );
				printf( "found animation!\n" );
			}
		
			BeginDrawing( );
			ClearBackground( LIME );
			
			DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , BLACK );
			EndDrawing( );

		}
	}

	CloseWindow( );

	idle( );
	return 1;
}
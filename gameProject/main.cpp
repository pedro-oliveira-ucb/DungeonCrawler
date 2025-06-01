#include <iostream>
#include <thread>
#include <sstream> 
#include <iomanip> 

#include "Game/gameControls/keybindHandler/keybindHandler.h"
#include "Game/Managers/gameResourceManager/gameResourceManager.h"

#include "Game/Handlers/shadersHandler/shadersHandler.h"

#include "Game/game.h"
#include "Game/gameRender/gameRender.h"
#include "Globals/Globals.h"
#include "Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>


Game game;

int main( void ) {
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	Log::Print( "[Render] Initialized window!" );

	SetConfigFlags( FLAG_VSYNC_HINT );
	//	raylib::SetConfigFlags( raylib::FLAG_FULLSCREEN_MODE ); // Define a flag de fullscreen
	InitWindow( globals.screenWidth , globals.screenHeight , "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen
	InitAudioDevice( );      // Initialize audio device
	DisableCursor( );

	Log::Print( "[main] starting game!" );
	if ( !game.start( ) ) {
		Log::Print( "[main] failed to start game!" );
	}
	else {

		while ( !game.isResourcesLoaded( ) ) {
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		}

		shaderHandler::Get( ).initializeRenderTexture( );
		shaderHandler::Get( ).preLoadAll( );

		float MusicVolume = 80.0f;
		float SoundVolume = 80.0f;

		while ( true )
		{
			if ( IsKeyPressed( KEY_ESCAPE ) ) { 
				globals.gamePaused = !globals.gamePaused;
			}

			if ( globals.gamePaused ) {
				_gameResourceManager.getMusicManager( )->pauseMusic( );
			}
			else {
				_gameResourceManager.getMusicManager( )->resumeMusic( );
			}

			_gameResourceManager.getSoundManager( )->setVolume( globals.SoundVolume );
			_gameResourceManager.getMusicManager( )->SetMusicVolume( globals.MusicVolume );

			Vector2 mousePos = GetMousePosition( );
			globals.mousePosX = mousePos.x;
			globals.mousePosY = mousePos.y;
			keybindHandler::Get( ).update( );
			gameRender::Get( ).processSoundEvents( );
			shaderHandler::Get( ).updateAll( );

			BeginDrawing( );
			ClearBackground( WHITE );

			gameRender::Get( ).render( );

			shaderHandler::Get( ).renderAll( );

			EndDrawing( );
		}
		CloseWindow( );
	}
	system( "pause" );

	return 1;
}
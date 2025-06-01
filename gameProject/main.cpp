#include <iostream>
#include <thread>
#include <sstream> 
#include <iomanip> 

#include "Game/gameControls/keybindHandler/keybindHandler.h"
#include "Game/gameObjects/gameState/mainMenuState/mainMenuState.h"
#include "Game/gameObjects/gameState/inGameState/inGameState.h"

#include "Game/Managers/gameResourceManager/gameResourceManager.h"
#include "Game/Managers/gameStateManagers/gameStateManager.h"

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
	//SetConfigFlags( FLAG_FULLSCREEN_MODE ); 
	InitWindow( Globals::Get( ).screenWidth , Globals::Get( ).screenHeight , "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen
	InitAudioDevice( );      // Initialize audio device
	DisableCursor( );

	gameStateManager gameStateManager_;
	// Define o estado inicial
   // gameStateManager.ChangeState(std::make_unique<MainMenuState>());
   // OU se MainMenuState não precisar de argumentos no construtor:

	BeginDrawing( );
	ClearBackground( BLACK );
	DrawText( "Loading" , Globals::Get( ).screenWidth / 2 - MeasureText( "Loading" , 20 ) / 2 , Globals::Get( ).screenHeight / 2 - 10 , 20 , WHITE );
	EndDrawing( );

	Log::Print( "[main] starting game!" );
	if ( !game.start( ) ) {
		Log::Print( "[main] failed to start game!" );
	}
	else {
		while ( !game.isResourcesLoaded( ) ) {
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		}

		gameStateManager_.ChangeState( std::make_unique<mainMenuState>( ) );

		shaderHandler::Get( ).initializeRenderTexture( );
		shaderHandler::Get( ).preLoadAll( );

		float MusicVolume = 80.0f;
		float SoundVolume = 80.0f;

		while ( true )
		{
			_gameResourceManager.getSoundManager( )->setVolume( Globals::Get().getGameSettings()->getSoundVolume() );
			_gameResourceManager.getMusicManager( )->setMusicVolume( Globals::Get( ).getGameSettings( )->getMusicVolume( ) );

			Vector2 mousePos = GetMousePosition( );
			Globals::Get().mousePosX = mousePos.x;
			Globals::Get().mousePosY = mousePos.y;

			// --- Entrada ---
			gameStateManager_.HandleInput( );

			// --- Atualização ---
			float deltaTime = GetFrameTime( );
			gameStateManager_.Update( deltaTime );

			keybindHandler::Get( ).update( );
			gameRender::Get( ).processSoundEvents( );
			shaderHandler::Get( ).updateAll( );

			BeginDrawing( );
			ClearBackground( WHITE );

			// --- Renderização ---
			gameStateManager_.Render( ); // O estado atual desenha o que precisa

			gameRender::Get( ).renderCustomCursor( );

			//Log::Print( "[main] current game time: %f" , Globals::Get( ).getGame( )->getCurrentGameTime( ) );

			EndDrawing( );
		}
		CloseWindow( );
	}
	system( "pause" );

	return 1;
}
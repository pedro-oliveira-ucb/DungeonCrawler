#include <iostream>
#include <thread>
#include <sstream>  // Necessário para std::stringstream
#include <iomanip>  // Necessário para std::setw e std::setfill


#include "Game/gameControls/keybindHandler/keybindHandler.h"
#include "Game/Managers/gameResourceManager/gameResourceManager.h"
#include "Game/game.h"
#include "Game/gameRender/gameRender.h"
#include "Globals/Globals.h"
#include "Utils/Log/Log.h"

Game game;

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>


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
		// Shader loading & generic preparing.
		Shader shader = LoadShader( 0 , "vignette.fs" );

		int rLoc = GetShaderLocation( shader , "radius" );
		int blurLoc = GetShaderLocation( shader , "blur" );
		int colLoc = GetShaderLocation( shader , "color" );

		// Radius and blur.
		float radius = -0.29;

		float blur = 1.1f;

		Vector3 vColor = { 0.f, 0.f, 0.f }; // Vignette color.

		RenderTexture2D vTexture = LoadRenderTexture( globals.screenWidth , globals.screenHeight ); // Vignette texture.

		waitGameLoad( );

		float MusicVolume = 80.0f;
		float SoundVolume = 80.0f;

		while ( true )
		{
			// Update values.
			if ( IsKeyDown( KEY_UP ) ) MusicVolume += 0.5f;
			if ( IsKeyDown( KEY_DOWN ) ) MusicVolume -= 0.5f;
			if ( IsKeyDown( KEY_RIGHT ) ) SoundVolume += 0.5f;
			if ( IsKeyDown( KEY_LEFT ) ) SoundVolume -= 0.5f;
			if ( IsKeyPressed( KEY_ESCAPE ) ) {
				globals.gamePaused = !globals.gamePaused;
				Log::Print( "Game Paused: %d" , globals.gamePaused );
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

			SetShaderValue( shader , rLoc , &radius , SHADER_UNIFORM_FLOAT );
			SetShaderValue( shader , blurLoc , &blur , SHADER_UNIFORM_FLOAT );
			SetShaderValue( shader , colLoc , &vColor , SHADER_UNIFORM_VEC3 );

			BeginDrawing( );
			ClearBackground( WHITE );
			gameRender::Get( ).render( );

			// Draw vignette.
			BeginShaderMode( shader );
			Rectangle sourceRec = { 0.0f, 0.0f, ( float ) vTexture.texture.width, ( float ) vTexture.texture.height };
			Vector2 destRec = { 0.0f, 0.0f };
			DrawTextureRec(
				vTexture.texture ,
				sourceRec ,
				destRec ,
				BLANK
			);
			EndShaderMode( );


			// Some information.
			DrawText( TextFormat( "Music Volume: %.2f" , globals.MusicVolume ) , 10 , 30 , 20 , WHITE );
			DrawText( TextFormat( "Sound volume: %.2f" , globals.SoundVolume ) , 10 , 60 , 20 , WHITE );

			//DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , BLACK );
			EndDrawing( );

		}
		CloseWindow( );
	}
	system( "pause" );

	idle( );
	return 1;
}
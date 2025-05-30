#include <iostream>
#include <thread>
#include <sstream>  // Necessário para std::stringstream
#include <iomanip>  // Necessário para std::setw e std::setfill


#include "Game/gameControls/keybindHandler/keybindHandler.h"
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
	game.start( );
	Log::Print( "[main] game started!" );


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

	while ( true )
	{
		// Update values.
		if ( IsKeyDown( KEY_UP ) ) radius += 0.01f;
		if ( IsKeyDown( KEY_DOWN ) ) radius -= 0.01f;

		if ( IsKeyDown( KEY_RIGHT ) ) blur += 0.01f;
		if ( IsKeyDown( KEY_LEFT ) ) blur -= 0.01f;

		Vector2 mousePos = GetMousePosition( );

		globals.mousePosX = mousePos.x;
		globals.mousePosY = mousePos.y;

		keybindHandler::Get( ).update( );

		gameRender::Get( ).processSoundEvents( );

		SetShaderValue( shader , rLoc , &radius , SHADER_UNIFORM_FLOAT );
		SetShaderValue( shader , blurLoc , &blur , SHADER_UNIFORM_FLOAT );
		SetShaderValue( shader , colLoc , &vColor , SHADER_UNIFORM_VEC3 );

		BeginDrawing( );
		//remove old draws?
		ClearBackground( WHITE );

		gameRender::Get( ).render( );

		// Draw vignette.
		BeginShaderMode( shader );

		Rectangle sourceRec = { 0.0f, 0.0f, ( float ) vTexture.texture.width, ( float ) vTexture.texture.height };
		Vector2 destRec = { 0.0f, 0.0f };

		DrawTextureRec(
		    vTexture.texture, 
			sourceRec ,
			destRec ,
		    BLANK
		);

		EndShaderMode( );
		// Some information.
		DrawText( TextFormat( "Radius: %.2f" , radius ) , 10 , 30 , 20 , WHITE );

		DrawText( TextFormat( "Blur: %.2f" , blur ) , 10 , 60 , 20 , WHITE );

		//DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , BLACK );
		EndDrawing( );
	}

	CloseWindow( );

	idle( );
	return 1;
}
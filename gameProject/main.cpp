#include <iostream>
#include <thread>
#include <sstream>  // Necessário para std::stringstream
#include <iomanip>  // Necessário para std::setw e std::setfill


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

void extractStyleSheet( std::string spritesheetPath ) {
	const int spriteWidth = 128;   // Largura de cada sprite
	const int spriteHeight = 128;  // Altura de cada sprite

	// Carrega o spritesheet
	Image spritesheet = LoadImage( spritesheetPath.c_str() );
	if ( spritesheet.data == nullptr ) {
		TraceLog( LOG_ERROR , "Spritesheet não encontrado!" );
		return;
	}

	int columns = spritesheet.width / spriteWidth;
	int rows = spritesheet.height / spriteHeight;
	int totalSprites = columns * rows;

	int spriteIndex = 0;
	for ( int y = 0; y < rows; y++ ) {
		for ( int x = 0; x < columns; x++ ) {
			Rectangle sourceRec = {
				( float ) x * spriteWidth,
				( float ) y * spriteHeight,
				( float ) spriteWidth,
				( float ) spriteHeight
			};

			// Extrai o sprite
			Image sprite = ImageFromImage( spritesheet , sourceRec );

			// Nome do arquivo com 3 dígitos (ex: sprite_001.png)
			std::stringstream filename;
			filename << spriteIndex << ".png";

			// Salva o sprite como imagem
			ExportImage( sprite , filename.str( ).c_str( ) );

			UnloadImage( sprite );
			spriteIndex++;
		}
	}

	// Libera recursos
	UnloadImage( spritesheet );
}

int main( void ) {

	Log::Print( "[Render] Initialized window!" );

	SetConfigFlags( FLAG_VSYNC_HINT );

	//	raylib::SetConfigFlags( raylib::FLAG_FULLSCREEN_MODE ); // Define a flag de fullscreen
	InitWindow( globals.screenWidth , globals.screenHeight , "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen
	InitAudioDevice( );      // Initialize audio device

	extractStyleSheet( "style.png" );

	DisableCursor( );

	
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

			gameRender::Get( ).soundEvents( );

			Vector2 mousePos = GetMousePosition( );

			globals.mousePosX = mousePos.x;
			globals.mousePosY = mousePos.y;


			BeginDrawing( );
			//remove old draws?
			ClearBackground( BLACK );

			gameRender::Get().render( );
			
			//DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , BLACK );
			EndDrawing( );

		}
	}

	CloseWindow( );

	idle( );
	return 1;
}

#include "../../../Globals/Globals.h"

namespace raylib {
#include <raylib/raylib.h>
}

bool InitGameWindow( ) {
	raylib::SetConfigFlags( raylib::FLAG_FULLSCREEN_MODE ); // Define a flag de fullscreen
	raylib::InitWindow( 0 , 0 , "Janela Fullscreen" ); // Tamanhos ignorados no fullscreen

	while ( !raylib::WindowShouldClose( ) )
	{
		raylib::BeginDrawing( );
		raylib::ClearBackground( raylib::RAYWHITE );
		// raylib::DrawText( "Janela em fullscreen!" , 100 , 100 , 20 , raylib::DARKGRAY );
		raylib::EndDrawing( );
	}

	raylib::CloseWindow( );

	globals.exitGame = true;
	return true;
}
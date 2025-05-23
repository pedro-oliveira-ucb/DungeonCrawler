#include "gameRender.h"

#include "../../Utils/Log/Log.h"

#include "../../Globals/Globals.h"

#include "RenderEntities/RenderEntities.h"

#include "../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../Managers/gameResourceManager/gameResourceManager.h"

RenderEntities entititiesRender;

#include <raylib/raylib.h>
#include <raylib/raymath.h>


Camera2D camera = { 0 };

bool check = false;

void gameRender::soundEvents( ) {

	std::string soundEvent = gameSoundsQueue.getLatestOnQueue( );
	while ( !soundEvent.empty( ) ) {

		if ( !_gameResourceManager.getSoundManager( )->playSound( soundEvent ) ) {
			Log::Print( "[soundEvents] cant play %s", soundEvent.c_str() );
		}

		soundEvent = gameSoundsQueue.getLatestOnQueue( );
	}
}

void gameRender::renderCustomCursor( ) {
	DrawCircle( GetMouseX( ) , GetMouseY( ) , 10 , BLUE );
}

void gameRender::render( ) {

	

	static float zoomLevel = 1.0f;

	// Zoom com scroll do mouse
	zoomLevel += GetMouseWheelMove( ) * 0.1f;
	zoomLevel = Clamp( zoomLevel , 1.f , 2.0f );
	camera.zoom = zoomLevel;


	CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
	GVector2D localPos = local->getEntityPosition( );
	if ( local != nullptr ) {
		// Define o ponto no mundo a ser seguido
		camera.target = { localPos.x, localPos.y };

		// Centraliza a câmera no meio da tela
		camera.offset = { GetScreenWidth( ) / 2.0f, GetScreenHeight( ) / 2.0f };
	}
	camera.rotation = 0.0f;

	BeginMode2D( camera );

	entititiesRender.render( );

	renderCustomCursor( );

	EndMode2D( );

}
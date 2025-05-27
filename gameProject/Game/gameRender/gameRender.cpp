#include "gameRender.h"

#include "RenderEntities/RenderEntities.h"

#include "../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../Managers/gameResourceManager/gameResourceManager.h"

#include "../../Utils/Log/Log.h"
#include "../../Globals/Globals.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

Camera2D camera = { 0 };

RenderEntities entititiesRender;

bool check = false;

void gameRender::soundEvents( ) {

	if ( IsKeyPressed( KEY_SPACE ) ) {
		_gameResourceManager.getMusicManager( )->playMusic( musicType::BossMusic );
	}

	std::string soundEvent = gameSoundsQueue.getLatestOnQueue( );
	while ( !soundEvent.empty( ) ) {

		if ( !_gameResourceManager.getSoundManager( )->playSound( soundEvent ) ) {
			Log::Print( "[soundEvents] cant play %s" , soundEvent.c_str( ) );
		}

		soundEvent = gameSoundsQueue.getLatestOnQueue( );
	}
	_gameResourceManager.getMusicManager( )->updateMusic( );
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
	static GVector2D oldLocalPos;
	if ( local != nullptr ) {

		GVector2D localPos = local->getEntityPosition( );

		Vector2 mousePos( { globals.mousePosX , globals.mousePosY } );

		// Corrige a posição do mouse com base no movimento do jogador
		if ( oldLocalPos.x != localPos.x || oldLocalPos.y != localPos.y ) {
			float deltaX = localPos.x - oldLocalPos.x;
			float deltaY = localPos.y - oldLocalPos.y;

			mousePos.x += deltaX;
			mousePos.y += deltaY;

			oldLocalPos = localPos;
		}

		// Corrige limites da posição do mouse
		mousePos.x = Clamp( mousePos.x , localPos.x - ( globals.screenWidth / 2.0f ) , localPos.x + ( globals.screenWidth / 2.0f ) );
		mousePos.y = Clamp( mousePos.y , localPos.y - ( globals.screenHeight / 2.0f ) , localPos.y + ( globals.screenHeight / 2.0f ) );

		SetMousePosition( mousePos.x , mousePos.y );


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
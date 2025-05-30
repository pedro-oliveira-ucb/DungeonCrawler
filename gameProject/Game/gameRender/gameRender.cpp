#include "gameRender.h"

#include "RenderEntities/RenderEntities.h"
#include "DialogsRender/DialogsRender.h"

#include "../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../gameObjects/gamdDialogHandler/gameDialogHandler.h"
#include "../Managers/gameResourceManager/gameResourceManager.h"

#include "../../Utils/Log/Log.h"
#include "../../Globals/Globals.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

Camera2D camera = { 0 };

RenderEntities entititiesRender;
DialogsRender dialogsRender;

bool check = false;

void gameRender::processSoundEvents( ) {

	if ( IsKeyPressed( KEY_SPACE ) ) {
		_gameResourceManager.getMusicManager( )->playMusic( musicType::BossMusic );
	}

	std::string soundEvent = gameSoundsEventHandler::Get().getLatestOnQueue( );
	while ( !soundEvent.empty( ) ) {

		if ( !_gameResourceManager.getSoundManager( )->playSound( soundEvent ) ) {
			Log::Print( "[soundEvents] cant play %s" , soundEvent.c_str( ) );
		}

		soundEvent = gameSoundsEventHandler::Get( ).getLatestOnQueue( );
	}
	_gameResourceManager.getMusicManager( )->updateMusic( );
}

void gameRender::renderCustomCursor( ) {
	DrawCircle( globals.mousePosX , globals.mousePosY , 10 , BLUE );
}

void gameRender::renderDialogs( ) {
	dialogsRender.render( );
}

void gameRender::correctMousePosition( ) {

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
}

void gameRender::render( ) {
	
	if ( IsKeyPressed( KEY_SPACE ) ) {
		gameDialog newDialog;
		newDialog.dialogText = "Hello World!";
		newDialog.dialogStayTime = 2;
		newDialog.dialogDuration = 10;
		newDialog.triggers.emplace_back( timedTrigger( "localPlayer_hurt" , 2 ) );
		gameDialogHandler::Get( ).throwDialog( newDialog );
	}

	// Zoom com scroll do mouse
	zoomLevel += GetMouseWheelMove( ) * 0.1f;
	zoomLevel = Clamp( zoomLevel , 1.f , 2.0f );
	camera.zoom = zoomLevel;
	correctMousePosition( );
	camera.rotation = 0.0f;

	BeginMode2D( camera );
	{
		entititiesRender.render( );	
		renderCustomCursor( );
	}
	EndMode2D( );
	renderDialogs( );
}
#include "inGameState.h"

#include "../../CameraController/CameraController.h"

#include "../mainMenuState/mainMenuState.h"
#include "../inGameOverState/inGameOverState.h"

#include "../../../gameObjects/gameMap/gameMap.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" 
#include "../../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Managers/LevelManager/LevelManager.h"

#include "../../../Handlers/shadersHandler/shadersHandler.h"
#include "../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../../../Handlers/gamdDialogHandler/gameDialogHandler.h"
#include "../../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"
#include "../../../../Globals/playerStats/playerStats.h"

#include "../../../gameRender/gameRender.h"

#include "../../../../Globals/Globals.h"
#include "../../../../Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>


inGameState::inGameState( ) {
	Log::Print( "[inGameState]: Constructor" );
}

inGameState::~inGameState( ) {
	Log::Print( "[inGameState]: Destructor" );
}

void inGameState::OnEnter( gameStateManager * manager ) {
	Log::Print( "[inGameState]: OnEnter" );
	levelManager.initialize( );
	Log::Print( "[inGameState]: levelManager initialized" );
	levelManager.start( );
	Log::Print( "[inGameState]: levelManager started" );

	Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PLAYING );
	_gameResourceManager.getMusicManager( )->playMusic( musicType::DungeonMusic , 5 );
	showUpgradeScreen = false;

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	GVector2D localPos = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );
	if ( player != nullptr ) {
		localPos = player->getEntityPosition( );
	}
	Vector2 playerPos = { localPos.x , localPos.y };
	CameraController::Get( ).initialize( playerPos , 1.0f );

	this->setEntering( true );
}

void inGameState::OnExit( gameStateManager * manager ) {
	Log::Print( "[inGameState]: OnExit" );
	showUpgradeScreen = false;
}


void inGameState::HandleInput( gameStateManager * manager ) {
	if ( showUpgradeScreen ) {
		Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PAUSED );
		if ( IsKeyPressed( KEY_U ) || IsKeyPressed( KEY_ESCAPE ) ) {
			showUpgradeScreen = false;
			Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PLAYING );
		}
	}
	else {
		if ( IsKeyPressed( KEY_ESCAPE ) ) {
			currentGameState state = Globals::Get( ).getGame( )->getCurrentGameState( );
			currentGameState newState = state == currentGameState::GAME_STATE_PAUSED ? currentGameState::GAME_STATE_PLAYING : currentGameState::GAME_STATE_PAUSED;
			Globals::Get( ).getGame( )->setCurrentGameState( newState );
		}

		// Check if the player is in a corridor and if the upgrade screen should be shown
		if ( IsKeyPressed( KEY_U ) && Globals::Get( ).getGame( )->isInCorridor( ) ) {
			showUpgradeScreen = true;
			Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PAUSED );
			gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_screen_open" );
			return;
		}

		if ( Globals::Get( ).getGame( )->getCurrentGameState( ) == currentGameState::GAME_STATE_PAUSED && !showUpgradeScreen ) {
			return;
		}

		static float lastDialogThrowTime = 0.0f;

		GVector2D localPos = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );
		std::map<GVector2D , DoorInstanceData> doors = gameMap::Get( ).getDoorInstancesCopy( );
		if ( !doors.empty( ) ) {

			float gameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );
			for ( const auto & door : doors ) {
				if ( door.first.y > localPos.y ) {
					if ( door.second.unlocked )
						gameSoundsEventHandler::Get( ).addEventToQueue( "door_close" );

					gameMap::Get( ).getDoorInstanceData( door.first )->unlocked = false;
					continue;
				}

				if ( IsKeyPressed( KEY_E ) && door.first.distTo( localPos ) <= 100 ) {
					if ( Globals::Get( ).getGame( )->getNumKeys( ) >= 1 ) {
						Globals::Get( ).getGame( )->removeKey( 1 );
						gameMap::Get( ).getDoorInstanceData( door.first )->unlocked = true;
						gameSoundsEventHandler::Get( ).addEventToQueue( "door_open" );
					}
					else {
						if ( gameTime - lastDialogThrowTime > 5.0f ) {
							gameDialog lockedDialog;
							lockedDialog.dialogText = "You need a key to open this door!";
							lockedDialog.dialogDuration = 2;
							lockedDialog.dialogStayTime = 1;
							gameDialogHandler::Get( ).throwDialog( lockedDialog );
							lastDialogThrowTime = gameTime;
						}

						gameSoundsEventHandler::Get( ).addEventToQueue( "door_locked" );
					}
				}
			}
		}
	}
}

void inGameState::Update( gameStateManager * manager , float deltaTime ) {
	float EXIT_FADE_DURATION = 5;

	gameStateTransitionState stateTransition = this->updateStateTransition( deltaTime );

	currentGameState state = Globals::Get( ).getGame( )->getCurrentGameState( );

	switch ( state ) {
	case currentGameState::GAME_STATE_PAUSED:
		_gameResourceManager.getMusicManager( )->pauseMusic( );
		break;
	case currentGameState::GAME_STATE_PLAYING:
		Globals::Get( ).getGame( )->updateCurrentGameTime( deltaTime );
		break;
	case currentGameState::GAME_STATE_EXIT:
		if ( _gameResourceManager.getMusicManager( )->getcurrentMusicType( ) != musicType::MainMenuMusic )
			_gameResourceManager.getMusicManager( )->playMusic( musicType::MainMenuMusic , EXIT_FADE_DURATION / 2 );

		this->setExiting( true );
		break;
	case currentGameState::GAME_STATE_GAME_OVER:
		this->setExiting( true );
		break;
	default:
		break;
	}

	if ( state != currentGameState::GAME_STATE_PAUSED )
		_gameResourceManager.getMusicManager( )->resumeMusic( );

	switch ( stateTransition ) {
	case gameStateTransitionState::EXIT_FINISHED:

		if ( state == currentGameState::GAME_STATE_EXIT )
			manager->ChangeState( std::make_unique<mainMenuState>( ) );
		else if ( state == currentGameState::GAME_STATE_GAME_OVER )
			manager->ChangeState( std::make_unique<inGameOverState>( ) );

		break;
	}

	shaderHandler::Get( ).updateAll( );
	CameraController::Get( ).Update( deltaTime );
}



void inGameState::Render( gameStateManager * manager ) {
	ClearBackground( BLACK );

	Camera2D * cameraPointer = CameraController::Get( ).GetCamera( );

	if ( cameraPointer != nullptr ) {

		BeginMode2D( *cameraPointer );
		{
			gameRender::Get( ).renderMap( );
			gameRender::Get( ).renderEntities( );
			gameRender::Get( ).renderMapDoors( );
		}
		EndMode2D( );	
	}

	shaderHandler::Get( ).renderAll( );
	gameRender::Get( ).renderDialogs( );

	if ( Globals::Get( ).getGame( )->getCurrentGameState( ) == currentGameState::GAME_STATE_PAUSED && !showUpgradeScreen ) {
		gameRender::Get( ).renderPauseMenu( );
	}

	if ( showUpgradeScreen ) {
		gameRender::Get( ).renderShopMenu( );
	}
	else {
		gameRender::Get( ).renderHUD( );
	}

	this->renderTransition( manager );
}
#include "inGameState.h"

#include "../mainMenuState/mainMenuState.h"

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
	_gameResourceManager.getMusicManager( )->playMusic( musicType::DungeonMusic , 10 );
	showUpgradeScreen = false;
	this->setEntering( true );
}

void inGameState::OnExit( gameStateManager * manager ) {
	Log::Print( "[inGameState]: OnExit" );
	showUpgradeScreen = false;
}

void inGameState::setCameraPosition( ) {
	GVector2D localPos = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );
	camera.target = { localPos.x, localPos.y };
	camera.offset = { GetScreenWidth( ) / 2.0f, GetScreenHeight( ) / 2.0f };

	Vector2 mouseWorld = GetScreenToWorld2D( GetMousePosition( ) , camera );
	Globals::Get( ).mousePosWorldX = mouseWorld.x;
	Globals::Get( ).mousePosWorldY = mouseWorld.y;
}

void inGameState::HandleInput( gameStateManager * manager ) {
	if ( showUpgradeScreen ) {
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

		if ( IsKeyPressed( KEY_U ) ) {
			showUpgradeScreen = true;
			Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PAUSED );

			gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_screen_open" );
		}

		if ( Globals::Get( ).getGame( )->getCurrentGameState( ) == currentGameState::GAME_STATE_PAUSED && !showUpgradeScreen ) {
			return;
		}

		static float lastDialogThrowTime = 0.0f;

		GVector2D localPos = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );
		std::map<GVector2D , DoorInstanceData> doors = gameMap::Get( ).getDoorInstancesCopy( );
		if ( !doors.empty( ) ) {
			for ( const auto & door : doors ) {
				if ( door.first.y > localPos.y ) {
					if ( door.second.unlocked )
						gameSoundsEventHandler::Get( ).addEventToQueue( "door_close" );

					gameMap::Get( ).getDoorInstanceData( door.first )->unlocked = false;
					continue;
				}

				if ( door.first.distTo( localPos ) <= 100 && IsKeyPressed( KEY_E ) ) {
					if ( Globals::Get( ).getGame( )->getNumKeys( ) >= 1 ) {
						Globals::Get( ).getGame( )->removeKey( 1 );
						gameMap::Get( ).getDoorInstanceData( door.first )->unlocked = true;
						gameSoundsEventHandler::Get( ).addEventToQueue( "door_open" );
					}
					else {
						if ( GetTime( ) - lastDialogThrowTime > 5.0f ) {
							gameDialog lockedDialog;
							lockedDialog.dialogText = "You need a key to open this door!";
							lockedDialog.dialogDuration = 20;
							lockedDialog.dialogStayTime = 1;
							gameDialogHandler::Get( ).throwDialog( lockedDialog );
							lastDialogThrowTime = GetTime( );
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
	default:
		break;
	}

	if ( state != currentGameState::GAME_STATE_PAUSED )
		_gameResourceManager.getMusicManager( )->resumeMusic( );

	switch ( stateTransition ) {
	case gameStateTransitionState::EXIT_FINISHED:
		manager->ChangeState( std::make_unique<mainMenuState>( ) );
		break;
	}

	shaderHandler::Get( ).updateAll( );
	Globals::Get( ).getPlayerStats( )->updatePlayer( );
}

void inGameState::Render( gameStateManager * manager ) {
	ClearBackground( BLACK );

	zoomLevel += GetMouseWheelMove( ) * 0.1f;
	zoomLevel = Clamp( zoomLevel , 1.f , 2.0f );
	camera.zoom = zoomLevel;
	setCameraPosition( );
	camera.rotation = 0.0f;

	BeginMode2D( camera );
	{
		gameRender::Get( ).renderMap( );
		gameRender::Get( ).renderEntities( );
		gameRender::Get( ).renderMapDoors( );
	}
	EndMode2D( );

	shaderHandler::Get( ).renderAll( );
	gameRender::Get( ).renderDialogs( );

	if ( Globals::Get( ).getGame( )->getCurrentGameState( ) == currentGameState::GAME_STATE_PAUSED && !showUpgradeScreen ) {
		gameRender::Get( ).renderPauseMenu( );
	}

	if ( showUpgradeScreen ) {
		gameRender::Get( ).renderShopMenu( );
	}


	this->renderTransition( manager );
}
#include "inGameState.h"

#include "../mainMenuState/mainMenuState.h"

#include "../../../gameObjects/gameMap/gameMap.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transições de estado
#include "../../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Managers/LevelManager/LevelManager.h"

#include "../../../Handlers/shadersHandler/shadersHandler.h"
#include "../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

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
}

void inGameState::OnExit( gameStateManager * manager ) {
	// Descarregar recursos específicos do menu para liberar memória
	Log::Print( "[inGameState]: OnExit" );
}

void inGameState::setCameraPosition( ) {
	CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
	GVector2D localPos = local->getEntityPosition( );
	static GVector2D oldLocalPos;
	if ( local != nullptr ) {

		GVector2D localPos = local->getEntityPosition( );
		// Define o ponto no mundo a ser seguido
		camera.target = { localPos.x, localPos.y };
		// Centraliza a câmera no meio da tela
		camera.offset = { GetScreenWidth( ) / 2.0f, GetScreenHeight( ) / 2.0f };

		// Converte mouse para coordenadas do mundo
		Vector2 mouseWorld = GetScreenToWorld2D( GetMousePosition( ) , camera );
		GVector2D mousePosWorld( mouseWorld.x , mouseWorld.y );
		Globals::Get( ).mousePosWorldX = mousePosWorld.x;
		Globals::Get( ).mousePosWorldY = mousePosWorld.y;
	}
}

void inGameState::HandleInput( gameStateManager * manager ) {
	if ( IsKeyPressed( KEY_ESCAPE ) ) {
		currentGameState state = Globals::Get( ).getGame( )->getCurrentGameState( );
		currentGameState newState = state == currentGameState::GAME_STATE_PAUSED ? currentGameState::GAME_STATE_PLAYING : currentGameState::GAME_STATE_PAUSED;
		Globals::Get( ).getGame( )->setCurrentGameState( newState );
	}
	if ( IsKeyPressed( KEY_E ) ) {
		CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
		if ( local != nullptr ) {
			GVector2D localPos = local->getEntityPosition( );
			std::map<GVector2D , DoorInstanceData> doors = gameMap::Get( ).getDoorInstancesCopy( );
			if ( !doors.empty( ) ) {
				for ( const auto & door : doors ) {
					if ( door.second.unlocked )
						continue;

					if ( door.first.distTo( localPos ) <= 100 ) {
						gameMap::Get( ).getDoorInstanceData( door.first )->unlocked = true;
						gameSoundsEventHandler::Get( ).addEventToQueue( "door_open" );
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
}

void inGameState::Render( gameStateManager * manager ) {

	ClearBackground( BLACK );

	// Zoom com scroll do mouse
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
	//renderDialogs( );

	if ( Globals::Get( ).getGame( )->getCurrentGameState( ) == currentGameState::GAME_STATE_PAUSED ) {
		gameRender::Get( ).renderPauseMenu( );
	}

	shaderHandler::Get( ).renderAll( );

	this->renderTransition( manager );
}
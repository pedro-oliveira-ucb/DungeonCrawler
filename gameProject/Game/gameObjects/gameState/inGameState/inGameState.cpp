#include "inGameState.h"

#include "../mainMenuState/mainMenuState.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transições de estado
#include "../../../Managers/gameResourceManager/gameResourceManager.h"

#include "../../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../../gameRender/gameRender.h"

#include "../../../../Globals/Globals.h"
#include "../../../../Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>


inGameState::inGameState( ) {
	// Construtor (evite carregar recursos pesados aqui, prefira OnEnter)
	Log::Print( "[inGameState]: Constructor" );
}

inGameState::~inGameState( ) {
	// Destrutor (evite descarregar recursos pesados aqui, prefira OnExit)
	Log::Print( "[inGameState]: Destructor" );
}

void inGameState::OnEnter( gameStateManager * manager ) {
	Log::Print( "[inGameState]: OnEnter" );
	Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PLAYING );
	_gameResourceManager.getMusicManager( )->playMusic( musicType::DungeonMusic , 10 );

	// Carregar recursos específicos do menu: fontes, texturas, sons
	// Exemplo:
	// font = LoadFont("resources/myfont.png");
	// titleTexture = LoadTexture("resources/title.png");
	// Inicializar posições de botões, etc.
}

void inGameState::OnExit( gameStateManager * manager ) {
	Log::Print( "[inGameState]: OnExit" );
	// Descarregar recursos específicos do menu para liberar memória
	// Exemplo:
	// UnloadFont(font);
	// UnloadTexture(titleTexture);
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
		Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PAUSED );
	}
}

void inGameState::Update( gameStateManager * manager , float deltaTime ) {

	float EXIT_FADE_DURATION = 5;
	float alphaIncrementThisFrame = ( 255.0f / EXIT_FADE_DURATION ) * deltaTime;

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

		currentExitFadeAlpha += alphaIncrementThisFrame;

		// Clamp the alpha value to a maximum of 255.0f.
		if ( currentExitFadeAlpha > 255.0f ) {
			currentExitFadeAlpha = 255.0f;
			manager->ChangeState( std::make_unique<mainMenuState>( ) );
		}
		break;
	default:

		break;
	}

	if ( state != currentGameState::GAME_STATE_PAUSED )
		_gameResourceManager.getMusicManager( )->resumeMusic( );
}

void inGameState::exitGame( gameStateManager * manager ) {
	// Prepare the color for the fade rectangle.
	// Alpha component is cast to unsigned char, as typically expected by color structures.
	Color col = { 0, 0, 0, static_cast< unsigned char >( currentExitFadeAlpha ) };

	// Draw the fade rectangle.
	DrawRectangle( 0 , 0 , Globals::Get( ).screenWidth , Globals::Get( ).screenHeight , col );
}


void inGameState::Render( gameStateManager * manager ) {

	// Zoom com scroll do mouse
	zoomLevel += GetMouseWheelMove( ) * 0.1f;
	zoomLevel = Clamp( zoomLevel , 1.f , 2.0f );
	camera.zoom = zoomLevel;
	setCameraPosition( );
	camera.rotation = 0.0f;

	BeginMode2D( camera );
	{
		gameRender::Get( ).renderEntities( );
	}
	EndMode2D( );
	//renderDialogs( );

	switch ( Globals::Get( ).getGame( )->getCurrentGameState( ) ) {
	case currentGameState::GAME_STATE_PAUSED:
		gameRender::Get( ).renderPauseMenu( );
		break;
	case currentGameState::GAME_STATE_EXIT:
		exitGame( manager );
		break;
	}
}
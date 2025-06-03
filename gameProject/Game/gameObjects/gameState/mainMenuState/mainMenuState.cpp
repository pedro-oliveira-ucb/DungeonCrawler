
#include "mainMenuState.h"

#include <raylib/raylib.h>
#include "../inGameState/inGameState.h"

#include "../../../../Globals/Globals.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transições de estado
#include "../../../Managers/gameResourceManager/gameResourceManager.h" // Para transições de estado
#include "../../../SDK/Events/EventManager.h"
#include "../../../../Utils/Log/Log.h"




mainMenuState::mainMenuState( ) {
	// Construtor (evite carregar recursos pesados aqui, prefira OnEnter)
	Log::Print( "[mainMenuState]: Constructor" );

}

mainMenuState::~mainMenuState( ) {
	// Destrutor (evite descarregar recursos pesados aqui, prefira OnExit)
	Log::Print( "[mainMenuState]: Destructor" );
}

void mainMenuState::OnEnter( gameStateManager * manager ) {
	Log::Print( "[mainMenuState]: OnEnter" );

	this->setEntering( true );

	Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_NONE );
	if ( _gameResourceManager.getMusicManager( )->getcurrentMusicType( ) != musicType::MainMenuMusic )
		_gameResourceManager.getMusicManager( )->playMusic( musicType::MainMenuMusic , 1 );

	
	// Carregar recursos específicos do menu: fontes, texturas, sons
	// Exemplo:
	// font = LoadFont("resources/myfont.png");
	// titleTexture = LoadTexture("resources/title.png");
	// Inicializar posições de botões, etc.
}

void mainMenuState::OnExit( gameStateManager * manager ) {
	Log::Print( "[mainMenuState]: OnExit" );
	// Descarregar recursos específicos do menu para liberar memória
	// Exemplo:
	// UnloadFont(font);
	// UnloadTexture(titleTexture);
}

void mainMenuState::HandleInput( gameStateManager * manager ) {
	if ( IsKeyPressed( KEY_ENTER ) ) {
		std::cout << "MainMenuState: ENTER pressionado, mudando para InGame" << std::endl;
		EventManager::Get( ).Trigger( "menu_gameStart" );
		this->setExiting( true );
	}
	if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
		Vector2 mousePos = GetMousePosition( );
		// Lógica para verificar cliques em botões
		// if (CheckCollisionPointRec(mousePos, playButtonRect)) {
		//    manager->ChangeState(new InGameSta_example());
		// }
	}
}

void mainMenuState::Update( gameStateManager * manager , float deltaTime ) {
	// Lógica de atualização do menu (animações de botões, etc.)

	gameStateTransitionState transitionState = this->updateStateTransition( deltaTime );
	switch ( transitionState ) {
		case gameStateTransitionState::EXIT_FINISHED:
			manager->ChangeState( std::make_unique<inGameState>( ) );
		break;
	}
}

void mainMenuState::Render( gameStateManager * manager ) {
	ClearBackground( RAYWHITE );
	// Desenhar elementos do menu
	// DrawTexture(titleTexture, SCREEN_WIDTH / 2 - titleTexture.width / 2, 100, WHITE);
	DrawText( "MENU PRINCIPAL" , 190 , 200 , 20 , LIGHTGRAY );
	DrawText( "Pressione ENTER para iniciar" , 200 , 250 , 20 , DARKGRAY );
	this->renderTransition( manager );
}
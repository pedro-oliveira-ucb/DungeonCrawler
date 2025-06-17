#include "inGameOverState.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h"
#include "../../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Managers/LevelManager/LevelManager.h" // Adicionado para reiniciar o nível

#include "../mainMenuState/mainMenuState.h"
#include "../inGameState/inGameState.h" // Adicionado para reiniciar o jogo

#include "../../../gameRender/gameRender.h"
#include "../../../gameRender/components/components.h" // Adicionado para usar DrawButton

#include "../../../../Globals/Globals.h"
#include "../../../../Globals/playerStats/playerStats.h" // Adicionado para resetar estatísticas do jogador
#include "../../../../Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>


inGameOverState::inGameOverState( ) {
	Log::Print( "[inGameOverState]: Constructor" );
}

inGameOverState::~inGameOverState( ) {
	Log::Print( "[inGameOverState]: Destructor" );
}

void inGameOverState::OnEnter( gameStateManager * manager ) {
	Log::Print( "[inGameOverState]: OnEnter" );
	_gameResourceManager.getMusicManager( )->playMusic( musicType::gameOverMusic , 5 );
	// Resetar estatísticas do jogador ao entrar na tela de Game Over, se necessário
	// playerStats::Get().resetStats(); // Exemplo, descomente e ajuste conforme necessário

	this->setEntering( true );
}	

void inGameOverState::OnExit( gameStateManager * manager ) {
	Log::Print( "[inGameOverState]: OnExit" );
}

void inGameOverState::HandleInput( gameStateManager * manager ) {
	
}

void inGameOverState::Update( gameStateManager * manager , float deltaTime ) {
	// A transição de estado agora é tratada pelos botões em HandleInput
	// gameStateTransitionState stateTransition = this->updateStateTransition( deltaTime );
	// switch ( stateTransition ) {
	// case gameStateTransitionState::EXIT_FINISHED:
		// manager->ChangeState( std::make_unique<mainMenuState>( ) );
		// break;
	// }
	this->updateStateTransition(deltaTime); // Ainda processa o fade in/out do estado
}

void inGameOverState::Render( gameStateManager * manager ) {
	ClearBackground( BLACK ); // Fundo sombrio

	const static float screenWidth = static_cast<float>(GetScreenWidth( ));
	const static float screenHeight = static_cast<float>(GetScreenHeight( ));
	const static char * gameOverText = "Game Over";
	const static int gameOverTextFontSize = 70;
	const static float gameOverTextWidth = MeasureText( gameOverText , gameOverTextFontSize );
	const static float buttonWidth = 200;
	const static float buttonHeight = 50;
	const static float spacing = 20; // Espaçamento entre os botões e o texto

	// Botão Reiniciar Jogo
	const static Rectangle restartButtonBounds = {
		screenWidth / 2 - buttonWidth / 2,
		screenHeight / 2 - buttonHeight / 2,
		buttonWidth,
		buttonHeight
	};

	// Botão Menu Principal
	const static Rectangle mainMenuButtonBounds = {
		screenWidth / 2 - buttonWidth / 2,
		screenHeight / 2 + buttonHeight / 2 + spacing,
		buttonWidth,
		buttonHeight
	};


	DrawText( gameOverText , screenWidth / 2 - gameOverTextWidth / 2 , screenHeight / 4 - gameOverTextFontSize / 2 , gameOverTextFontSize , RAYWHITE );

	if ( components::Get( ).DrawButton( restartButtonBounds , "Restart Game" , WHITE , BLACK ) ) {
		manager->ChangeState( std::make_unique<inGameState>( ) );
	}

	if ( components::Get( ).DrawButton( mainMenuButtonBounds , "Main Menu" , WHITE , BLACK ) ) {
		manager->ChangeState( std::make_unique<mainMenuState>( ) );
	}

	this->renderTransition( manager );
}
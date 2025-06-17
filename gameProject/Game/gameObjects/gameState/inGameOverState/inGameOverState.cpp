#include "inGameOverState.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h"
#include "../../../Managers/gameResourceManager/gameResourceManager.h"

#include "../mainMenuState/mainMenuState.h"

#include "../../../gameRender/gameRender.h"

#include "../../../../Globals/Globals.h"
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


	this->setEntering( true );
}

void inGameOverState::OnExit( gameStateManager * manager ) {
	Log::Print( "[inGameOverState]: OnExit" );
}

void inGameOverState::HandleInput( gameStateManager * manager ) {
	
}

void inGameOverState::Update( gameStateManager * manager , float deltaTime ) {
	float EXIT_FADE_DURATION = 5;

	gameStateTransitionState stateTransition = this->updateStateTransition( deltaTime );

	currentGameState state = Globals::Get( ).getGame( )->getCurrentGameState( );

	switch ( stateTransition ) {
	case gameStateTransitionState::EXIT_FINISHED:
		manager->ChangeState( std::make_unique<mainMenuState>( ) );
		break;
	}
}

void inGameOverState::Render( gameStateManager * manager ) {
	ClearBackground( WHITE );


	this->renderTransition( manager );
}
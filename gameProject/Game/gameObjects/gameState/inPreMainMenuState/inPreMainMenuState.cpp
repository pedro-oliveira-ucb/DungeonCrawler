#include "inPreMainMenuState.h"

#include <raylib/raylib.h>
#include "../mainMenuState/mainMenuState.h"

#include "../../../../Globals/Globals.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transições de estado
#include "../../../Managers/gameResourceManager/gameResourceManager.h" // Para transições de estado
#include "../../../SDK/Events/EventManager.h"
#include "../../../../Utils/Log/Log.h"


inPreMainMenuState::inPreMainMenuState( ) {
	Log::Print( "[inPreMainMenuState]: Constructor" );
}

inPreMainMenuState::~inPreMainMenuState( ) {
	Log::Print( "[inPreMainMenuState]: Destructor" );
}

void inPreMainMenuState::OnEnter( gameStateManager * manager ) {
	Log::Print( "[inPreMainMenuState]: OnEnter" );
	Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_NONE );
	if ( _gameResourceManager.getMusicManager( )->getcurrentMusicType( ) != musicType::MainMenuMusic )
		_gameResourceManager.getMusicManager( )->playMusic( musicType::MainMenuMusic , 1 );

	std::vector<std::shared_ptr<rSprite>> * renderSprite = _gameResourceManager.getSpritesManager( )->getSprite( "gameProductor_0" );
	if(renderSprite == nullptr || renderSprite->empty()) {
		Log::Print("[inPreMainMenuState]: Sprite 'gameProductor_0' not found.");
		return;
	}

	this->renderTexturePointer = renderSprite->at( 0 )->getTexture( );

	this->setEntering( true );
}

void inPreMainMenuState::OnExit( gameStateManager * manager ) {
	Log::Print( "[inPreMainMenuState]: OnExit" );
}

void inPreMainMenuState::HandleInput( gameStateManager * manager ) {
	
}

void inPreMainMenuState::Update( gameStateManager * manager , float deltaTime ) {
	gameStateTransitionState transitionState = this->updateStateTransition( deltaTime );
	switch ( transitionState ) {
	case gameStateTransitionState::EXIT_FINISHED:
		manager->ChangeState( std::make_unique<mainMenuState>( ) );
		break;
	}

	this->m_runningTime += deltaTime;
	if ( this->m_runningTime > 5.0f ) {
		this->setExiting( true );
	}
}

void inPreMainMenuState::Render( gameStateManager * manager ) {
	ClearBackground( BLACK );
	// Desenhar elementos do menu
	// DrawTexture(titleTexture, SCREEN_WIDTH / 2 - titleTexture.width / 2, 100, WHITE);
	if ( this->renderTexturePointer != nullptr )
	{
		Texture2D * titleTexture = reinterpret_cast<Texture2D *>(this->renderTexturePointer);

		DrawTexture( *titleTexture , GetScreenWidth( ) / 2 - titleTexture->width / 2 , GetScreenHeight( ) / 2 - titleTexture->height / 2 , WHITE );
	}

	this->renderTransition( manager );
}
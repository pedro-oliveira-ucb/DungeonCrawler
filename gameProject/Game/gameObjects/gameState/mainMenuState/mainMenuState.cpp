
#include "mainMenuState.h"

#include <raylib/raylib.h>
#include "../inPreGameState/inPreGameState.h"

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
		manager->ChangeState( std::make_unique<inPreGameState>( ) );
		break;
	}

	static auto backgroundsprites = _gameResourceManager.getSpritesManager( )->getClip( "mainMenu_background" );

	if ( backgroundsprites != nullptr ) {
		// initialize backgroundTexture if not already done
		if ( this->backgroundTexture == nullptr ) {
			this->backgroundTexture = reinterpret_cast< Texture2D * >( backgroundsprites->getFrame( currentBackgroundIndex )->getTexture( ) );
			lastTimeSinceBackgroundUpdate = 0.0f;
		}
		else {

			if ( lastTimeSinceBackgroundUpdate > 0.1 )
			{
				currentBackgroundIndex++;

				if( currentBackgroundIndex >= backgroundsprites->size() ) {
					currentBackgroundIndex = 0; // Reset to the first frame
				}

				if(backgroundsprites ->getFrame( currentBackgroundIndex ) == nullptr) 
					Log::Print( "[mainMenuState]: Warning: Frame is null at index %d", currentBackgroundIndex  );				
				else {
					this->backgroundTexture = reinterpret_cast< Texture2D * >( backgroundsprites->getFrame( currentBackgroundIndex )->getTexture( ) );
					lastTimeSinceBackgroundUpdate = 0.0f;
				}
			}

			lastTimeSinceBackgroundUpdate += deltaTime;
		}
	}
}

void mainMenuState::Render( gameStateManager * manager ) {
	ClearBackground( BLACK );
	// Desenhar elementos do menu
	// DrawTexture(titleTexture, SCREEN_WIDTH / 2 - titleTexture.width / 2, 100, WHITE);

	float screenWidth = static_cast< float >( GetScreenWidth( ) );
	float screenHeight = static_cast< float >( GetScreenHeight( ) );

	if ( this->backgroundTexture != nullptr ) {
		// Define o retângulo de origem (a imagem inteira)
		Rectangle sourceRec = { 0.0f, 0.0f, ( float ) this->backgroundTexture->width, ( float ) this->backgroundTexture->height };

		// Define o retângulo de destino (a tela inteira)
		Rectangle destRec = { 0.0f, 0.0f,screenWidth, screenHeight };

		// Define a origem da transformação (para rotação/escala, pode ser {0, 0} para este caso)
		Vector2 origin = { 0, 0 };

		// Desenha a textura esticando-a para preencher o retângulo de destino
		DrawTexturePro(
			*this->backgroundTexture , // A textura
			sourceRec ,                 // Retângulo de origem
			destRec ,                   // Retângulo de destino (aqui está a mágica!)
			origin ,                    // Origem para transformações
			0.0f ,                      // Rotação
			WHITE                      // Cor (tint)
		);
	}

	float textPadding = 0.1;
	{
		std::string Text = "Main Menu";
		float textWidth = MeasureText( Text.c_str( ) , 20 ); // Medir a largura do texto
		float textX = ( screenWidth - textWidth ) / 2; // Centralizar horizontalmente
		float textY = 0 + screenHeight * textPadding;
		DrawText( Text.c_str( ) , static_cast< int >( textX ) , textY , 20 , WHITE );
	}

	{
		std::string Text = "Press ENTER to play";
		float textWidth = MeasureText( Text.c_str( ) , 20 ); // Medir a largura do texto
		float textX = ( screenWidth - textWidth ) / 2; // Centralizar horizontalmente
		float textY = 0 + screenHeight * ( 1 - textPadding );
		DrawText( Text.c_str( ) , static_cast< int >( textX ) , textY , 20 , WHITE );
	}

	this->renderTransition( manager );
}
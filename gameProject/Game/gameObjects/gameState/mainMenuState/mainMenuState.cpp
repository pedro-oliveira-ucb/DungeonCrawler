
#include "mainMenuState.h"

#include <raylib/raylib.h>
#include "../inGameState/inGameState.h"

#include "../../../../Globals/Globals.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transições de estado
#include "../../../Managers/gameResourceManager/gameResourceManager.h" // Para transições de estado
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
    Globals::Get( ).getCurrentGameState( )->setCurrentGameState( currentGameStateType::GAME_STATE_NONE );
    _gameResourceManager.getMusicManager( )->playMusic( musicType::MainMenuMusic, 1 );

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
        manager->ChangeState( std::make_unique<inGameState>( ) );

        // Exemplo de transição para o estado de jogo
        // manager->ChangeState(new InGameSta_example()); // Cuidado com memory leak, veja GameStateManager
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
}

void mainMenuState::Render( gameStateManager * manager ) {
    ClearBackground( RAYWHITE );
    // Desenhar elementos do menu
    // DrawTexture(titleTexture, SCREEN_WIDTH / 2 - titleTexture.width / 2, 100, WHITE);
    DrawText( "MENU PRINCIPAL" , 190 , 200 , 20 , LIGHTGRAY );
    DrawText( "Pressione ENTER para iniciar" , 200 , 250 , 20 , DARKGRAY );
    // Desenhar botões, etc.
}
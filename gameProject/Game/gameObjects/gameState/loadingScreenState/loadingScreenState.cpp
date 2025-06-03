#include "loadingScreenState.h"

#include <raylib/raylib.h>

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transições de estado
#include "../../../../Utils/Log/Log.h"

loadingScreenState::loadingScreenState( ) {
    // Construtor (evite carregar recursos pesados aqui, prefira OnEnter)
    Log::Print( "[loadingScreenState]: Constructor" );
}

loadingScreenState::~loadingScreenState( ) {
    // Destrutor (evite descarregar recursos pesados aqui, prefira OnExit)
    Log::Print( "[loadingScreenState]: Destructor" );
}

void loadingScreenState::OnEnter( gameStateManager * manager ) {
    Log::Print( "[loadingScreenState]: OnEnter" );
    // Carregar recursos específicos do menu: fontes, texturas, sons
    // Exemplo:
    // font = LoadFont("resources/myfont.png");
    // titleTexture = LoadTexture("resources/title.png");
    // Inicializar posições de botões, etc.
}

void loadingScreenState::OnExit( gameStateManager * manager ) {
    Log::Print( "[loadingScreenState]: OnExit" );
    // Descarregar recursos específicos do menu para liberar memória
    // Exemplo:
    // UnloadFont(font);
    // UnloadTexture(titleTexture);
}

void loadingScreenState::HandleInput( gameStateManager * manager ) {
    if ( IsKeyPressed( KEY_ENTER ) ) {
        std::cout << "loadingScreenState: ENTER pressionado, mudando para InGame" << std::endl;
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

void loadingScreenState::Update( gameStateManager * manager , float deltaTime ) {
    // Lógica de atualização do menu (animações de botões, etc.)
}

void loadingScreenState::Render( gameStateManager * manager ) {
    ClearBackground( RAYWHITE );
    // Desenhar elementos do menu
    // DrawTexture(titleTexture, SCREEN_WIDTH / 2 - titleTexture.width / 2, 100, WHITE);
    DrawText( "MENU PRINCIPAL" , 190 , 200 , 20 , LIGHTGRAY );
    DrawText( "Pressione ENTER para iniciar" , 200 , 250 , 20 , DARKGRAY );

    // Desenhar botões, etc.

    this->renderTransition( manager );
}
#include "loadingScreenState.h"

#include <raylib/raylib.h>

#include "../../../Managers/gameStateManagers/gameStateManager.h" // Para transi��es de estado
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
    // Carregar recursos espec�ficos do menu: fontes, texturas, sons
    // Exemplo:
    // font = LoadFont("resources/myfont.png");
    // titleTexture = LoadTexture("resources/title.png");
    // Inicializar posi��es de bot�es, etc.
}

void loadingScreenState::OnExit( gameStateManager * manager ) {
    Log::Print( "[loadingScreenState]: OnExit" );
    // Descarregar recursos espec�ficos do menu para liberar mem�ria
    // Exemplo:
    // UnloadFont(font);
    // UnloadTexture(titleTexture);
}

void loadingScreenState::HandleInput( gameStateManager * manager ) {
    if ( IsKeyPressed( KEY_ENTER ) ) {
        std::cout << "loadingScreenState: ENTER pressionado, mudando para InGame" << std::endl;
        // Exemplo de transi��o para o estado de jogo
        // manager->ChangeState(new InGameSta_example()); // Cuidado com memory leak, veja GameStateManager
    }
    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        Vector2 mousePos = GetMousePosition( );
        // L�gica para verificar cliques em bot�es
        // if (CheckCollisionPointRec(mousePos, playButtonRect)) {
        //    manager->ChangeState(new InGameSta_example());
        // }
    }
}

void loadingScreenState::Update( gameStateManager * manager , float deltaTime ) {
    // L�gica de atualiza��o do menu (anima��es de bot�es, etc.)
}

void loadingScreenState::Render( gameStateManager * manager ) {
    ClearBackground( RAYWHITE );
    // Desenhar elementos do menu
    // DrawTexture(titleTexture, SCREEN_WIDTH / 2 - titleTexture.width / 2, 100, WHITE);
    DrawText( "MENU PRINCIPAL" , 190 , 200 , 20 , LIGHTGRAY );
    DrawText( "Pressione ENTER para iniciar" , 200 , 250 , 20 , DARKGRAY );

    // Desenhar bot�es, etc.

    this->renderTransition( manager );
}
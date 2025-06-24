#include "inPreGameState.h"

#include "../../../Handlers/gamdDialogHandler/gameDialogHandler.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h"
#include "../../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Managers/LevelManager/LevelManager.h"

#include "../mainMenuState/mainMenuState.h"
#include "../inGameState/inGameState.h"

#include "../../../gameRender/gameRender.h"
#include "../../../gameRender/components/components.h"

#include "../../../../Globals/Globals.h"
#include "../../../../Globals/playerStats/playerStats.h"
#include "../../../../Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

inPreGameState::inPreGameState( ) {
    Log::Print( "[inPreGameState]: Constructor" );
}

inPreGameState::~inPreGameState( ) {
    Log::Print( "[inPreGameState]: Destructor" );
}

void inPreGameState::OnEnter( gameStateManager * manager ) {
    Log::Print( "[inPreGameState]: OnEnter" );

    // Fila de diálogos para contar a história do cãozinho mágico.
    // Presume-se que o gameDialogHandler exibe os diálogos em sequência (FIFO).
    gameDialogHandler & dialogHandler = gameDialogHandler::Get( );

    // NOTA: Se o seu handler não limpa diálogos ao mudar de estado,
    // pode ser uma boa ideia chamar uma função como `dialogHandler.clearDialogs()` aqui.

    gameDialog d1;
    d1.dialogText = "Era uma vez, um cachorrinho magico chamado Sparky...";
    d1.dialogDuration = 3.0f;
    d1.dialogStayTime = 1.0f;
    dialogHandler.throwDialog( d1 );

    gameDialog d2;
    d2.dialogText = "Ele era o guardiao de um artefato de poder imenso: o Osso Dourado.";
    d2.dialogDuration = 3.0f;
    d2.dialogStayTime = 1.0f;
    dialogHandler.throwDialog( d2 );

    gameDialog d3;
    d3.dialogText = "Forcas sombrias cobicavam o osso, querendo usar seu poder para o mal.";
    d3.dialogDuration = 3.0f;
    d3.dialogStayTime = 1.0f;
    dialogHandler.throwDialog( d3 );

    gameDialog d4;
    d4.dialogText = "Para protege-lo, Sparky decidiu esconde-lo no lugar mais seguro que conhecia...";
    d4.dialogDuration = 3.0f;
    d4.dialogStayTime = 1.0f;
    dialogHandler.throwDialog( d4 );

    gameDialog d5;
    d5.dialogText = "...as profundezas de uma masmorra traiçoeira.";
    d5.dialogDuration = 3.0f;
    d5.dialogStayTime = 1.0f;
    dialogHandler.throwDialog( d5 );

    gameDialog d6;
    d6.dialogText = "Ele precisa derrotar todos os inimigos para garantir que o osso fique seguro para sempre.";
    d6.dialogDuration = 3.0f;
    d6.dialogStayTime = 1.0f;
    dialogHandler.throwDialog( d6 );

    gameDialog d7;
    d7.dialogText = "Ajude Sparky em sua nobre missao!";
    d7.dialogDuration = 3.0f;
    d7.dialogStayTime = 1.0f; // Um tempo extra no final
    dialogHandler.throwDialog( d7 );

    this->setEntering( true );
}

void inPreGameState::OnExit( gameStateManager * manager ) {
    Log::Print( "[inPreGameState]: OnExit" );
}

void inPreGameState::HandleInput( gameStateManager * manager ) {
    // Permite que o jogador pule os diálogos pressionando Enter, Espaço ou clicando.
    if ( IsKeyPressed( KEY_ENTER ) || IsKeyPressed( KEY_SPACE ) || IsMouseButtonPressed( MOUSE_LEFT_BUTTON ) ) {
        // Pula todos os diálogos restantes para iniciar o jogo imediatamente.
        // Se o seu handler tiver uma função para pular um de cada vez (ex: skipCurrentDialog),
        // você pode usá-la aqui para um efeito mais gradual.
        if ( gameDialogHandler::Get( ).hasActiveDialog( ) ) {
            gameDialogHandler::Get( ).clearDialogs( ); // Limpa a fila inteira.
        }
    }
}

void inPreGameState::Update( gameStateManager * manager , float deltaTime ) {
    gameStateTransitionState transitionState = this->updateStateTransition( deltaTime );

    Globals::Get( ).getGame( )->updateCurrentGameTime( deltaTime );

    switch ( transitionState ) {
    case gameStateTransitionState::EXIT_FINISHED:
        // Transita para o estado de jogo principal
        manager->ChangeState( std::make_unique<inGameState>( ) );
        break;
    }

    // Quando todos os diálogos forem exibidos (ou pulados), inicia a transição de saída.
    if ( !gameDialogHandler::Get( ).hasActiveDialog( ) && !isExiting( ) ) {
        setExiting( true );
    }
}

void inPreGameState::Render( gameStateManager * manager ) {
    ClearBackground( BLACK ); // Fundo escuro e misterioso para a introdução da lore

    // Renderiza a caixa de diálogo atual
    gameRender::Get( ).renderDialogs( );

    // Exibe uma dica para o jogador sobre como pular a introdução
    if ( gameDialogHandler::Get( ).hasActiveDialog( ) ) {
        const char * skipText = "Pressione [ESPACO] ou [ENTER] para pular";
        int textWidth = MeasureText( skipText , 20 );
        DrawText( skipText , GetScreenWidth( ) / 2 - textWidth / 2 , GetScreenHeight( ) - 40 , 20 , GRAY );
    }

    // Renderiza o efeito de transição de tela (fade in/out)
    this->renderTransition( manager );
}

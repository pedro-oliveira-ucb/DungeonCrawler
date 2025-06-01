#include "gameStateManager.h"

#include "../../gameObjects/gameState/gameState.h" // Agora podemos incluir
#include "../../../Utils/Log/Log.h"

gameStateManager::gameStateManager( ) : running( true ) {
    Log::Print( "[GameStateManager]: Initialized" );
}

gameStateManager::~gameStateManager( ) {
    Log::Print( "[GameStateManager]: Cleaning up states" );
    // Limpa todos os estados restantes na pilha
    while ( !states.empty( ) ) {
        states.top( )->OnExit( this );
        states.pop( );
    }
    Log::Print( "[GameStateManager]: Destroyed" );
}

void gameStateManager::ChangeState( std::unique_ptr<gameState> state ) {
    if ( !states.empty( ) ) {
        states.top( )->OnExit( this ); // Chama OnExit do estado antigo
        states.pop( );               // Remove o estado antigo
    }
    states.push( std::move( state ) );  // Adiciona o novo estado
    if ( !states.empty( ) ) {
        states.top( )->OnEnter( this ); // Chama OnEnter do novo estado
    }
}

void gameStateManager::PushState( std::unique_ptr<gameState> state ) {
    if ( !states.empty( ) ) {
        // Opcional: pausar ou notificar o estado abaixo
        // states.top()->OnPause();
    }
    states.push( std::move( state ) );
    if ( !states.empty( ) ) {
        states.top( )->OnEnter( this );
    }
}

void gameStateManager::PopState( ) {
    if ( !states.empty( ) ) {
        states.top( )->OnExit( this );
        states.pop( );
    }
    if ( !states.empty( ) ) {
        // Opcional: resumir o estado que voltou ao topo
        // states.top()->OnResume();
    }
    if ( states.empty( ) ) {
        QuitGame( ); // Fecha o jogo se não houver mais estados
    }
}

gameState * gameStateManager::GetCurrentState( ) const {
    if ( states.empty( ) ) {
        return nullptr;
    }
    return states.top( ).get( ); // Retorna o ponteiro bruto do unique_ptr
}

void gameStateManager::HandleInput( ) {
    gameState * currentState = GetCurrentState( );
    if ( currentState ) {
        currentState->HandleInput( this );
    }
}

void gameStateManager::Update( float deltaTime ) {
    gameState * currentState = GetCurrentState( );
    if ( currentState ) {
        currentState->Update( this , deltaTime );
    }
}

void gameStateManager::Render( ) {
    gameState * currentState = GetCurrentState( );
    if ( currentState ) {
        currentState->Render( this );
    }
    // Se você quiser que estados abaixo na pilha também renderizem (ex: jogo atrás do menu de pausa transparente)
    // você pode iterar pela pilha de trás para frente ou de uma forma específica.
    // Por simplicidade, aqui apenas o estado do topo renderiza.
}
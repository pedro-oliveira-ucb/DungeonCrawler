#pragma once

#include <memory> // Para std::unique_ptr
#include <stack>  // Para empilhar estados (ex: menu de pausa sobre o jogo)

// #include "GameState.h" // Não inclua aqui para evitar dependência circular se GameState inclui GameStateManager
class gameState; // Declaração avançada

class gameStateManager {
public:
    gameStateManager( );
    ~gameStateManager( );

    // Define o estado inicial ou substitui o estado atual
    void ChangeState( std::unique_ptr<gameState> state );

    // Adiciona um estado no topo da pilha (ex: menu de pausa)
    void PushState( std::unique_ptr<gameState> state );

    // Remove o estado do topo da pilha
    void PopState( );

    // Retorna o estado ativo (o do topo da pilha ou o único estado)
    gameState * GetCurrentState( ) const;

    // Delega as chamadas para o estado ativo
    void HandleInput( );
    void Update( float deltaTime );
    void Render( );

    bool IsRunning( ) const { return running; }
    void QuitGame( ) { running = false; }

private:
    std::stack<std::unique_ptr<gameState>> states;
    bool running;
};
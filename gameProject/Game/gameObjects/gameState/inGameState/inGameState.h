#pragma once
#include "../gameState.h"
#include <vector>
#include <string>
#include <functional>

// Forward declaration
class gameStateManager;


class inGameState : public gameState {
public:
    inGameState( );
    ~inGameState( ) override;

    void OnEnter( gameStateManager * manager ) override;
    void OnExit( gameStateManager * manager ) override;
    void HandleInput( gameStateManager * manager ) override;
    void Update( gameStateManager * manager , float deltaTime ) override;
    void Render( gameStateManager * manager ) override;
    gameStateType GetType( ) const override { return gameStateType::IN_GAME; }
private:
   
    int currentPlayerRoom = -1;

    // Lógica da tela de Upgrades integrada
    bool showUpgradeScreen = true;
};
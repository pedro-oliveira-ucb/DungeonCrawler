#pragma once
#include "../gameState.h"
#include <vector>
#include <string>
#include <functional>

// Forward declaration
class gameStateManager;

class inPreGameState : public gameState {
public:
    inPreGameState( );
    ~inPreGameState( ) override;

    void OnEnter( gameStateManager * manager ) override;
    void OnExit( gameStateManager * manager ) override;
    void HandleInput( gameStateManager * manager ) override;
    void Update( gameStateManager * manager , float deltaTime ) override;
    void Render( gameStateManager * manager ) override;
    gameStateType GetType( ) const override { return gameStateType::IN_GAME; }
private:

};
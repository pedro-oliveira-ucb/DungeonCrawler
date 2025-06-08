#pragma once
#include "../gameState.h"
#include <raylib/raylib.h>

class inGameState : public gameState {
public:
    inGameState( );
    ~inGameState( ) override;

    void OnEnter( gameStateManager * manager ) override;
    void OnExit( gameStateManager * manager ) override;
    void HandleInput( gameStateManager * manager ) override;
    void Update( gameStateManager * manager , float deltaTime ) override;
    void Render( gameStateManager * manager ) override;
    gameStateType GetType( ) const override { return gameStateType::LOADING; }
private:
    Camera2D camera = { 0 };
    void setCameraPosition( );
    bool isPaused = false;
    float zoomLevel = 0.0f;
    int currentPlayerRoom = -1;
};
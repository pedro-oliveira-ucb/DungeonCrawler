#pragma once
#include "../gameState.h"


class inPreMainMenuState : public gameState {
public:
    inPreMainMenuState( );
    ~inPreMainMenuState( ) override;

    void OnEnter( gameStateManager * manager ) override;
    void OnExit( gameStateManager * manager ) override;
    void HandleInput( gameStateManager * manager ) override;
    void Update( gameStateManager * manager , float deltaTime ) override;
    void Render( gameStateManager * manager ) override;
    gameStateType GetType( ) const override { return gameStateType::LOADING; }
private:
    float m_runningTime = 0.0f;
    void * renderTexturePointer = nullptr;
};
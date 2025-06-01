#pragma once
#include "../gameState.h"


class mainMenuState : public gameState {
public:
    mainMenuState( );
    ~mainMenuState( ) override;

    void OnEnter( gameStateManager * manager ) override;
    void OnExit( gameStateManager * manager ) override;
    void HandleInput( gameStateManager * manager ) override;
    void Update( gameStateManager * manager , float deltaTime ) override;
    void Render( gameStateManager * manager ) override;
    gameStateType GetType( ) const override { return gameStateType::MAIN_MENU; }

private:
   
};
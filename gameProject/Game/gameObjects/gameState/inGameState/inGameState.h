#pragma once
#include "../gameState.h"
#include <raylib/raylib.h>
#include <vector>
#include <string>
#include <functional>

// Forward declaration
class gameStateManager;

// Enum para identificar os stats de forma �nica para o switch-case
enum class PlayerStatIndex {
    MaxHealth,
    MaxMana,
    AttackDamageFactor,
    AttackSpeedFactor,
    Defense,
    MovementSpeed,
    // Adicione mais conforme necess�rio
};

// Estrutura para os bot�es de upgrade dentro do inGameState
struct UpgradeButtonInGame {
    std::string label;
    Rectangle rect;
    std::function<void()> action;
    int cost;
    bool canAfford;
    PlayerStatIndex statIndex; // Usar enum para identificar o stat
    float improvementAmount;    // Quanto o stat melhora
};

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
    Camera2D camera = { 0 };
    void setCameraPosition( );
    float zoomLevel = 1.0f; // Inicializado com um valor padr�o
    int currentPlayerRoom = -1;

    // L�gica da tela de Upgrades integrada
    bool showUpgradeScreen = false;
    std::vector<UpgradeButtonInGame> upgradeButtons;
    Font upgradeFont; // Usar GetFontDefault() ou carregar uma espec�fica

    void InitializeUpgradeButtons();
    void RenderUpgradeScreen();
    void HandleUpgradeScreenInput();
    void ApplyPlayerUpgrade(PlayerStatIndex statIdx, float improvementAmount, int manaCost); // Alterado para usar PlayerStatIndex
    bool CanPlayerAffordUpgrade(int cost); // Fun��o auxiliar para verificar custo
    void UpdateUpgradeButtonAffordability(); // Atualiza quais bot�es o jogador pode pagar
};
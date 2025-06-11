
#pragma once

#include "../gameState.h"
#include <vector>
#include <string>
#include <functional> // Para std::function

#include <raylib/raylib.h>

// Forward declaration
class gameStateManager;

struct UpgradeButton {
    std::string label;
    Rectangle rect;
    std::function<void()> action;
    int cost;
    bool canAfford;
};

class UpgradeScreenState : public gameState {
    std::vector<UpgradeButton> upgradeButtons;
    Font font; // Assumindo que você tem uma fonte carregada ou usa GetFontDefault()

    void InitializeButtons();
    void ApplyUpgrade(const std::string& statName, float improvementAmount, int manaCost);

public:
    UpgradeScreenState();
    ~UpgradeScreenState() override;

    void OnEnter(gameStateManager* manager) override;
    void OnExit(gameStateManager* manager) override;
    void HandleInput(gameStateManager* manager) override;
    void Update(gameStateManager* manager, float deltaTime) override;
    void Render(gameStateManager* manager) override;
    gameStateType GetType() const override;
};

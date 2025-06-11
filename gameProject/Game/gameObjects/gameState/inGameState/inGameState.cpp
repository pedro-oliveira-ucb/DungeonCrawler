#include "inGameState.h"

#include "../mainMenuState/mainMenuState.h"

#include "../../../gameObjects/gameMap/gameMap.h"

#include "../../../Managers/gameStateManagers/gameStateManager.h" 
#include "../../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Managers/LevelManager/LevelManager.h"

#include "../../../Handlers/shadersHandler/shadersHandler.h"
#include "../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../../../Handlers/gamdDialogHandler/gameDialogHandler.h"
#include "../../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"
#include "../../../../Globals/playerStats/playerStats.h"

#include "../../../gameRender/gameRender.h"

#include "../../../../Globals/Globals.h"
#include "../../../../Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

bool inGameState::CanPlayerAffordUpgrade(int cost) {
    CPlayerEntity* player = entitiesHandler::Get().getLocalPlayer();
    if (!player) {
        return false; // Placeholder para teste de UI
    }

	int currentMana = Globals::Get( ).getGame( )->getNumMana( );

	return currentMana >= cost;
}

void inGameState::UpdateUpgradeButtonAffordability() {
    for (auto& button : upgradeButtons) {
        button.canAfford = CanPlayerAffordUpgrade(button.cost);
    }
}

void inGameState::ApplyPlayerUpgrade(PlayerStatIndex statIdx, float improvementAmount, int manaCost) {
    playerStats* stats = Globals::Get().getPlayerStats();
    CPlayerEntity* player = entitiesHandler::Get().getLocalPlayer();

    if (!stats || !player) {
        Log::Print("[inGameState] Player stats ou player entity não encontrado para upgrade!");
        return;
    }

    if (!CanPlayerAffordUpgrade(manaCost)) {
        gameSoundsEventHandler::Get().addEventToQueue("upgrade_fail_mana");
        return;
    }

    Log::Print("[inGameState] TODO: Implementar dedução de mana para upgrade.");

    std::string statNameForLog = "Unknown";

    switch (statIdx) {
        case PlayerStatIndex::MaxHealth:
            stats->setMaxHealth(stats->getMaxHealth() + improvementAmount);
            statNameForLog = "MaxHealth";
            break;
        case PlayerStatIndex::MaxMana:
            stats->setMaxMana(stats->getMaxMana() + improvementAmount);
            statNameForLog = "MaxMana";
            break;
        case PlayerStatIndex::AttackDamageFactor:
            stats->setAttackDamageFactor(stats->getAttackDamageFactor() + improvementAmount);
            statNameForLog = "AttackDamageFactor";
            break;
        case PlayerStatIndex::AttackSpeedFactor:
            stats->setAttackSpeedFactor(stats->getAttackSpeedFactor() + improvementAmount);
            statNameForLog = "AttackSpeedFactor";
            break;
        case PlayerStatIndex::Defense:
            stats->setDefense(stats->getDefense() + improvementAmount);
            statNameForLog = "Defense";
            break;
        case PlayerStatIndex::MovementSpeed:
            stats->setMovementSpeed(stats->getMovementSpeed() + improvementAmount);
            statNameForLog = "MovementSpeed";
            break;
        default:
            Log::Print("[inGameState] Índice de stat de upgrade desconhecido!");
            return;
    }
    Log::Print("[inGameState] Upgrade %s aplicado. Custo: %d", statNameForLog.c_str(), manaCost);
    Globals::Get( ).getGame( )->removeMana( manaCost );
    gameSoundsEventHandler::Get().addEventToQueue("upgrade_success");

    UpdateUpgradeButtonAffordability();
}

void inGameState::InitializeUpgradeButtons() {
    upgradeButtons.clear();
    float screenWidth = static_cast<float>(GetScreenWidth());
    float buttonWidth = 280;
    float buttonHeight = 50;
    float spacing = 15;
    int initialY = 150;
    int currentY = initialY;

    auto addButton = [&](const std::string& label, PlayerStatIndex statIdx, float improvement, int cost) {
        upgradeButtons.push_back({
            label,
            { screenWidth / 2 - buttonWidth / 2, static_cast<float>(currentY), buttonWidth, buttonHeight },
            [this, statIdx, improvement, cost]() { ApplyPlayerUpgrade(statIdx, improvement, cost); },
            cost,
            CanPlayerAffordUpgrade(cost),
            statIdx,
            improvement
        });
        currentY += static_cast<int>(buttonHeight + spacing);
    };

    addButton("Vida Max (+10)", PlayerStatIndex::MaxHealth, 10.0f, 10);
    addButton("Mana Max (+10)", PlayerStatIndex::MaxMana, 10.0f, 10);
    addButton("Dano Ataque (+5%)", PlayerStatIndex::AttackDamageFactor, 0.05f, 15);
    addButton("Vel. Ataque (+5%)", PlayerStatIndex::AttackSpeedFactor, -0.05f, 15);
    addButton("Defesa (+2)", PlayerStatIndex::Defense, 2.0f, 20);
    addButton("Vel. Movimento (+10)", PlayerStatIndex::MovementSpeed, 10.0f, 10);

    UpdateUpgradeButtonAffordability();
}

void inGameState::HandleUpgradeScreenInput() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        for (const auto& button : upgradeButtons) {
            if (CheckCollisionPointRec(mousePoint, button.rect)) {
                if (button.canAfford) {
                    button.action();
                } else {
                    gameSoundsEventHandler::Get().addEventToQueue("upgrade_fail_mana");
                }
                break;
            }
        }
    }
    if (IsKeyPressed(KEY_U) || IsKeyPressed(KEY_ESCAPE)) {
        showUpgradeScreen = false;
        Globals::Get().getGame()->setCurrentGameState(currentGameState::GAME_STATE_PLAYING);
    }
}

void inGameState::RenderUpgradeScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    const char* title = "Melhorias";
    int titleFontSize = 40;
    Vector2 titleSize = MeasureTextEx(upgradeFont, title, titleFontSize, 2);
    DrawTextEx(upgradeFont, title, {(float)GetScreenWidth() / 2 - titleSize.x / 2, 50}, titleFontSize, 2, RAYWHITE);

    CPlayerEntity* player = entitiesHandler::Get().getLocalPlayer();
    std::string manaText = "Mana: [N/A]";
    if (player) {
		int currentMana = Globals::Get( ).getGame( )->getNumMana( );

        manaText = "Mana: [" + std::to_string(currentMana) + "]";
    }
    DrawTextEx(upgradeFont, manaText.c_str(), {20, 20}, 20, 1, YELLOW);

    for (const auto& button : upgradeButtons) {
        Color buttonColor = button.canAfford ? LIME : GRAY;
        Color textColor = BLACK;
        if (CheckCollisionPointRec(GetMousePosition(), button.rect)) {
            buttonColor = button.canAfford ? GREEN : DARKGRAY;
            if (!button.canAfford) textColor = LIGHTGRAY;
        }
        DrawRectangleRec(button.rect, buttonColor);
        
        std::string buttonText = button.label + " (C: " + std::to_string(button.cost) + ")";
        Vector2 textSize = MeasureTextEx(upgradeFont, buttonText.c_str(), 20, 1);
        DrawTextEx(upgradeFont, buttonText.c_str(), 
                 { button.rect.x + (button.rect.width - textSize.x) / 2,
                   button.rect.y + (button.rect.height - textSize.y) / 2 },
                 20, 1, textColor);
    }
    Vector2 closeHintSize = MeasureTextEx(upgradeFont, "Pressione [U] ou [ESC] para fechar", 20, 1);
    DrawTextEx(upgradeFont, "Pressione [U] ou [ESC] para fechar", { (float)GetScreenWidth() / 2 - closeHintSize.x / 2, (float)GetScreenHeight() - 40 }, 20, 1, WHITE);
}

inGameState::inGameState() {
    Log::Print("[inGameState]: Constructor");
    upgradeFont = GetFontDefault();
    InitializeUpgradeButtons();
}

inGameState::~inGameState() {
    Log::Print("[inGameState]: Destructor");
}

void inGameState::OnEnter(gameStateManager* manager) {
    Log::Print("[inGameState]: OnEnter");
    levelManager.initialize();
    Log::Print("[inGameState]: levelManager initialized");
    levelManager.start();
    Log::Print("[inGameState]: levelManager started");

    Globals::Get().getGame()->setCurrentGameState(currentGameState::GAME_STATE_PLAYING);
    _gameResourceManager.getMusicManager()->playMusic(musicType::DungeonMusic, 10);
    showUpgradeScreen = false;
    UpdateUpgradeButtonAffordability();
    this->setEntering(true);
}

void inGameState::OnExit(gameStateManager* manager) {
    Log::Print("[inGameState]: OnExit");
    showUpgradeScreen = false;
}

void inGameState::setCameraPosition() {
    GVector2D localPos = Globals::Get().getGame()->getCurrentLocalPlayerPosition();
    camera.target = { localPos.x, localPos.y };
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
    Globals::Get().mousePosWorldX = mouseWorld.x;
    Globals::Get().mousePosWorldY = mouseWorld.y;
}

void inGameState::HandleInput(gameStateManager* manager) {
    if (showUpgradeScreen) {
        HandleUpgradeScreenInput();
    } else {
        if (IsKeyPressed(KEY_ESCAPE)) {
            currentGameState state = Globals::Get().getGame()->getCurrentGameState();
            currentGameState newState = state == currentGameState::GAME_STATE_PAUSED ? currentGameState::GAME_STATE_PLAYING : currentGameState::GAME_STATE_PAUSED;
            Globals::Get().getGame()->setCurrentGameState(newState);
        }

        if (IsKeyPressed(KEY_U)) {
            showUpgradeScreen = true;
            Globals::Get().getGame()->setCurrentGameState(currentGameState::GAME_STATE_PAUSED);
            UpdateUpgradeButtonAffordability();
            gameSoundsEventHandler::Get().addEventToQueue("upgrade_screen_open");
        }

        if (Globals::Get().getGame()->getCurrentGameState() == currentGameState::GAME_STATE_PAUSED && !showUpgradeScreen) {
            return;
        }

        static float lastDialogThrowTime = 0.0f;

        GVector2D localPos = Globals::Get().getGame()->getCurrentLocalPlayerPosition();
        std::map<GVector2D, DoorInstanceData> doors = gameMap::Get().getDoorInstancesCopy();
        if (!doors.empty()) {
            for (const auto& door : doors) {
                if (door.first.y > localPos.y) {
                    if (door.second.unlocked)
                        gameSoundsEventHandler::Get().addEventToQueue("door_close");

                    gameMap::Get().getDoorInstanceData(door.first)->unlocked = false;
                    continue;
                }

                if (door.first.distTo(localPos) <= 100 && IsKeyPressed(KEY_E)) {
                    if (Globals::Get().getGame()->getNumKeys() >= 1) {
                        Globals::Get().getGame()->removeKey(1);
                        gameMap::Get().getDoorInstanceData(door.first)->unlocked = true;
                        gameSoundsEventHandler::Get().addEventToQueue("door_open");
                    } else {
                        if (GetTime() - lastDialogThrowTime > 5.0f) {
                            gameDialog lockedDialog;
                            lockedDialog.dialogText = "You need a key to open this door!";
                            lockedDialog.dialogDuration = 20;
                            lockedDialog.dialogStayTime = 1;
                            gameDialogHandler::Get().throwDialog(lockedDialog);
                            lastDialogThrowTime = GetTime();
                        }

                        gameSoundsEventHandler::Get().addEventToQueue("door_locked");
                    }
                }
            }
        }
    }
}

void inGameState::Update(gameStateManager* manager, float deltaTime) {
    float EXIT_FADE_DURATION = 5;

    gameStateTransitionState stateTransition = this->updateStateTransition(deltaTime);

    currentGameState state = Globals::Get().getGame()->getCurrentGameState();

    switch (state) {
    case currentGameState::GAME_STATE_PAUSED:
        _gameResourceManager.getMusicManager()->pauseMusic();
        break;
    case currentGameState::GAME_STATE_PLAYING:
        Globals::Get().getGame()->updateCurrentGameTime(deltaTime);
        break;
    case currentGameState::GAME_STATE_EXIT:
        if (_gameResourceManager.getMusicManager()->getcurrentMusicType() != musicType::MainMenuMusic)
            _gameResourceManager.getMusicManager()->playMusic(musicType::MainMenuMusic, EXIT_FADE_DURATION / 2);

        this->setExiting(true);
        break;
    default:
        break;
    }

    if (state != currentGameState::GAME_STATE_PAUSED)
        _gameResourceManager.getMusicManager()->resumeMusic();

    switch (stateTransition) {
    case gameStateTransitionState::EXIT_FINISHED:
        manager->ChangeState(std::make_unique<mainMenuState>());
        break;
    }

    shaderHandler::Get().updateAll();
}

void inGameState::Render(gameStateManager* manager) {
    ClearBackground(BLACK);

    zoomLevel += GetMouseWheelMove() * 0.1f;
    zoomLevel = Clamp(zoomLevel, 1.f, 2.0f);
    camera.zoom = zoomLevel;
    setCameraPosition();
    camera.rotation = 0.0f;

    BeginMode2D(camera);
    {
        gameRender::Get().renderMap();
        gameRender::Get().renderEntities();
        gameRender::Get().renderMapDoors();
    }
    EndMode2D();

    shaderHandler::Get().renderAll();
    gameRender::Get().renderDialogs();

    if (Globals::Get().getGame()->getCurrentGameState() == currentGameState::GAME_STATE_PAUSED && !showUpgradeScreen) {
        gameRender::Get().renderPauseMenu();
    }

    if (showUpgradeScreen) {
        RenderUpgradeScreen();
    }

    this->renderTransition(manager);
}
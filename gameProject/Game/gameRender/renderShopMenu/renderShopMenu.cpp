#include "renderShopMenu.h"

#include <string>
#include <functional>
#include <vector>

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"


#include <raylib/raylib.h>

// Estrutura para os botões de upgrade dentro do inGameState
struct UpgradeButtonInGame {
    std::string label;
    Rectangle rect;
    std::function<void( )> action;
    int cost;
    bool canAfford;
    PlayerStatIndex statIndex; // Usar enum para identificar o stat
    float improvementAmount;    // Quanto o stat melhora
};


std::vector<UpgradeButtonInGame> upgradeButtons;

bool CanPlayerAffordUpgrade( int cost ) {
	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	if ( !player ) {
		return false; // Placeholder para teste de UI
	}

	int currentMana = Globals::Get( ).getGame( )->getNumMana( );

	return currentMana >= cost;
}


void UpdateUpgradeButtonAffordability( ) {
	for ( auto & button : upgradeButtons ) {
		button.canAfford = CanPlayerAffordUpgrade( button.cost );
	}
}

void ApplyPlayerUpgrade( PlayerStatIndex statIdx , float improvementAmount , int manaCost ) {
	playerStats * stats = Globals::Get( ).getPlayerStats( );
	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );

	if ( !stats || !player ) {
		Log::Print( "[inGameState] Player stats ou player entity não encontrado para upgrade!" );
		return;
	}

	if ( !CanPlayerAffordUpgrade( manaCost ) ) {
		gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_fail_mana" );
		return;
	}


	std::string statNameForLog = "Unknown";

	switch ( statIdx ) {
	case PlayerStatIndex::MaxHealth:
		stats->setMaxHealth( stats->getMaxHealth( ) + ( stats->getMaxHealth( ) * ( improvementAmount / 100 ) ) );
		statNameForLog = "MaxHealth";
		break;
	case PlayerStatIndex::MaxStamina:
		stats->setMaxStamina( stats->getMaxStamina( ) + improvementAmount );
		statNameForLog = "MaxStamina";
		break;
	case PlayerStatIndex::StaminaLoss:
		stats->setStaminaLossFactor( stats->getStaminaLossFactor( ) + improvementAmount );
		statNameForLog = "StaminaLoss";
		break;
	case PlayerStatIndex::StaminaRegen:
		stats->setStaminaRegenFactor( stats->getStaminaRegenFactor( ) + improvementAmount );
		statNameForLog = "StaminaRegen";
		break;
	case PlayerStatIndex::AttackDamageFactor:
		stats->setAttackDamageFactor( stats->getAttackDamageFactor( ) + improvementAmount );
		statNameForLog = "AttackDamageFactor";
		break;
	case PlayerStatIndex::AttackSpeedFactor:
		stats->setAttackSpeedFactor( stats->getAttackSpeedFactor( ) + improvementAmount );
		statNameForLog = "AttackSpeedFactor";
		break;
	case PlayerStatIndex::Defense:
		stats->setDefense( stats->getDefense( ) + improvementAmount );
		statNameForLog = "Defense";
		break;
	case PlayerStatIndex::MovementSpeed:
		stats->setMovementSpeed( stats->getMovementSpeed( ) + improvementAmount );
		statNameForLog = "MovementSpeed";
		break;
	default:
		Log::Print( "[inGameState] Índice de stat de upgrade desconhecido!" );
		return;
	}
	Log::Print( "[inGameState] Upgrade %s aplicado. Custo: %d" , statNameForLog.c_str( ) , manaCost );
	Globals::Get( ).getGame( )->removeMana( manaCost );
	gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_success" );

	UpdateUpgradeButtonAffordability( );
}

void HandleUpgradeScreenInput( ) {
	if ( IsMouseButtonPressed( MOUSE_LEFT_BUTTON ) ) {
		Vector2 mousePoint = GetMousePosition( );
		for ( const auto & button : upgradeButtons ) {
			if ( CheckCollisionPointRec( mousePoint , button.rect ) ) {
				if ( button.canAfford ) {
					button.action( );
				}
				else {
					gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_fail_mana" );
				}
				break;
			}
		}
	}
}

bool initialized = false;

void initialize() {

	upgradeButtons.clear( );
	float screenWidth = static_cast< float >( GetScreenWidth( ) );
	float buttonWidth = 280;
	float buttonHeight = 50;
	float spacing = 15;
	int initialY = 150;
	int currentY = initialY;

	auto addButton = [ & ] ( const std::string & label , PlayerStatIndex statIdx , float improvement , int cost ) {
		upgradeButtons.push_back( {
			label,
			{ screenWidth / 2 - buttonWidth / 2, static_cast< float >( currentY ), buttonWidth, buttonHeight },
			[ statIdx, improvement, cost ] ( ) { ApplyPlayerUpgrade( statIdx, improvement, cost ); },
			cost,
			CanPlayerAffordUpgrade( cost ),
			statIdx,
			improvement
			} );
		currentY += static_cast< int >( buttonHeight + spacing );
		};

	addButton( "Vida Max (5%)" , PlayerStatIndex::MaxHealth , 5.f , 60 );
	addButton( "Vel. Movimento (+5)" , PlayerStatIndex::MovementSpeed , 5.0f , 60 );
	addButton( "Vel. Recuperação Stamina (+10)" , PlayerStatIndex::StaminaRegen , 10.0f , 60 );
	addButton( "Vel. Perda Stamina (-10)" , PlayerStatIndex::StaminaRegen , 10.0f , 60 );

	//addButton( "Dano Ataque (+5%)" , PlayerStatIndex::AttackDamageFactor , 0.05f , 15 );
	//addButton( "Dano Ataque (+5%)" , PlayerStatIndex::AttackDamageFactor , 0.05f , 15 );
	//addButton( "Vel. Ataque (+5%)" , PlayerStatIndex::AttackSpeedFactor , -0.05f , 15 );
	//addButton( "Defesa (+2)" , PlayerStatIndex::Defense , 2.0f , 20 );


	UpdateUpgradeButtonAffordability( );
	initialized = true;
}

void renderShopMenu::render( ) {
	if ( !initialized ) {
		initialize( );
	}

	UpdateUpgradeButtonAffordability( );

	DrawRectangle( 0 , 0 , GetScreenWidth( ) , GetScreenHeight( ) , Fade( BLACK , 0.7f ) );

	const char * title = "Melhorias";
	int titleFontSize = 40;
	Font font = GetFontDefault( );

	Vector2 titleSize = MeasureTextEx( font , title , titleFontSize , 2 );
	DrawTextEx( font , title , { ( float ) GetScreenWidth( ) / 2 - titleSize.x / 2, 50 } , titleFontSize , 2 , RAYWHITE );

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	std::string manaText = "Mana: [N/A]";
	if ( player ) {
		int currentMana = Globals::Get( ).getGame( )->getNumMana( );

		manaText = "Mana: [" + std::to_string( currentMana ) + "]";
	}
	DrawTextEx( font , manaText.c_str( ) , { 20, 20 } , 20 , 1 , YELLOW );

	for ( const auto & button : upgradeButtons ) {
		Color buttonColor = button.canAfford ? LIME : GRAY;
		Color textColor = BLACK;
		if ( CheckCollisionPointRec( GetMousePosition( ) , button.rect ) ) {
			buttonColor = button.canAfford ? GREEN : DARKGRAY;
			if ( !button.canAfford ) textColor = LIGHTGRAY;
		}
		DrawRectangleRec( button.rect , buttonColor );

		std::string buttonText = button.label + " (C: " + std::to_string( button.cost ) + ")";
		Vector2 textSize = MeasureTextEx( font , buttonText.c_str( ) , 20 , 1 );
		DrawTextEx( font , buttonText.c_str( ) ,
			{ button.rect.x + ( button.rect.width - textSize.x ) / 2,
			  button.rect.y + ( button.rect.height - textSize.y ) / 2 } ,
			20 , 1 , textColor );
	}
	Vector2 closeHintSize = MeasureTextEx( font , "Pressione [U] ou [ESC] para fechar" , 20 , 1 );
	DrawTextEx( font , "Pressione [U] ou [ESC] para fechar" , { ( float ) GetScreenWidth( ) / 2 - closeHintSize.x / 2, ( float ) GetScreenHeight( ) - 40 } , 20 , 1 , WHITE );

	HandleUpgradeScreenInput( );
}
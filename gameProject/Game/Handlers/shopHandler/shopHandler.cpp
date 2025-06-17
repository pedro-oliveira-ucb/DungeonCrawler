#include "shopHandler.h"
#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

// Helper para obter textura de ícone
void * getIconTexture( const std::string & iconName ) {
	std::vector<std::shared_ptr<rSprite>> * spriteVector =
		_gameResourceManager.getSpritesManager( )->getSprite( "items_icons_" + iconName + "_0" );

	if ( !spriteVector || spriteVector->empty( ) ) {
		Log::Print( "Warning: Icon not found: %s" , iconName.c_str( ) );
		return nullptr;
	}
	return spriteVector->at( 0 )->getTexture( );
}

float getStatValue( PlayerStatIndex index , playerStats * status ) {
	switch ( index ) {
	case PlayerStatIndex::HealthDropProbability:
		return status->getHealthDropProbability( );
	case PlayerStatIndex::ManaDropProbability:
		return status->getManaDropProbability( );
	case PlayerStatIndex::MaxHealth:
		return status->getMaxHealth( );
	case PlayerStatIndex::MovementSpeed:
		return status->getMovementSpeed( );
	case PlayerStatIndex::MeleeAttackDamageFactor:
		return status->getMeleeAttackDamageFactor( );
	case PlayerStatIndex::MeleeAttackSpeedFactor:
		return status->getMeleeAttackSpeedFactor( );
	case PlayerStatIndex::MeleeAttackRangeFactor:
		return status->getMeleeAttackRangeFactor( );
	case PlayerStatIndex::RangeAttackDamageFactor:
		return status->getRangeAttackDamageFactor( );
	case PlayerStatIndex::RangeAttackSpeedFactor:
		return status->getRangeAttackSpeedFactor( );
	case PlayerStatIndex::RangeAttackRangeFactor:
		return status->getRangeAttackRangeFactor( );
	case PlayerStatIndex::Defense:
		return status->getDefense( );
	default:
		return 0.0f;
	}
}

void setStatValue( StatBonus bonus , playerStats * status ) {
	switch ( bonus.stat ) {
	case PlayerStatIndex::HealthDropProbability:
		status->setHealthDropProbability( bonus.value );
		break;
	case PlayerStatIndex::ManaDropProbability:
		status->setManaDropProbability( bonus.value );
		break;
	case PlayerStatIndex::MaxHealth:
		status->setMaxHealth( bonus.value );
		break;
	case PlayerStatIndex::MovementSpeed:
		status->setMovementSpeed( bonus.value );
		break;
	case PlayerStatIndex::MeleeAttackDamageFactor:
		status->setMeleeAttackDamageFactor( bonus.value );
		break;
	case PlayerStatIndex::MeleeAttackSpeedFactor:
		status->setMeleeAttackSpeedFactor( bonus.value );
		break;
	case PlayerStatIndex::MeleeAttackRangeFactor:
		status->setMeleeAttackRangeFactor( bonus.value );
		break;
	case PlayerStatIndex::RangeAttackDamageFactor:
		status->setRangeAttackDamageFactor( bonus.value );
		break;
	case PlayerStatIndex::RangeAttackSpeedFactor:
		status->setRangeAttackSpeedFactor( bonus.value );
		break;
	case PlayerStatIndex::RangeAttackRangeFactor:
		status->setRangeAttackRangeFactor( bonus.value );
		break;
	case PlayerStatIndex::Defense:
		status->setDefense( bonus.value );
		break;
	}
}

// Obter stats base do jogador
playerStats getBasePlayerStats( ) {
	playerStats base;

	base.setMaxHealth( 100.0f );
	base.setMaxStamina( 100.0f );

	base.setStaminaLossFactor( 50.0f );
	base.setStaminaRegenFactor( 25.0f );

	base.setMeleeAttackDamageFactor( 20.0f );
	base.setMeleeAttackSpeedFactor( 30.f );
	base.setMeleeAttackRangeFactor( 50.0f );

	base.setRangeAttackDamageFactor( 50.0f );
	base.setRangeAttackSpeedFactor( 150.f );
	base.setRangeAttackRangeFactor( 700.0f );

	base.setDefense( 5.0f );
	base.setMovementSpeed( 100.0f );

	return base;
}

shopHandler::shopHandler( ) : playerInventory( INVENTORY_SLOTS , -1 ) {
	categoryNames = {
		{ItemCategory::Starter, "Starter"},
		{ItemCategory::Utility, "Utility"},
		{ItemCategory::Offensive, "Offensive"},
		{ItemCategory::Defensive, "Defensive"},
		{ItemCategory::Hybrid, "Hybrid"},
		{ItemCategory::Legendary, "Legendary"}
	};
}

ShopItem * shopHandler::getSelectedItem( ) const {
	return selectedItem;
}

void shopHandler::setSelectedItem( ShopItem * item ) {
	selectedItem = item;
}

ItemCategory shopHandler::getSelectedCategory( ) const {
	return selectedCategory;
}

void shopHandler::setSelectedCategory( ItemCategory category ) {
	selectedCategory = category;
}

const std::map<ItemCategory , std::string> & shopHandler::getCategoryNames( ) const {
	return categoryNames;
}

const std::map<ItemCategory , std::vector<ShopItem>> & shopHandler::getCategorizedItems( ) const {
	return categorizedItems;
}

const std::vector<ShopItem> & shopHandler::getAllItems( ) const {
	return allItemsById;
}

const std::vector<int> & shopHandler::getPlayerInventory( ) const {
	return playerInventory;
}

void shopHandler::buySelectedItem( ) {
	std::lock_guard<std::mutex> lock( shopMutex );

	if ( !selectedItem ) {
		Log::Print( "[Shop] No item selected for purchase." );
		return;
	}

	if ( selectedItem->isOwned ) {
		Log::Print( "[Shop] Player already owns the item '%s'." , selectedItem->name.c_str( ) );
		gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_fail_mana" );
		return;
	}

	int currentMana = Globals::Get( ).getGame( )->getNumMana( );
	if ( currentMana < selectedItem->cost ) {
		Log::Print( "[Shop] Insufficient mana to buy '%s'." , selectedItem->name.c_str( ) );
		gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_fail_mana" );
		return;
	}

	int freeSlot = -1;
	for ( size_t i = 0; i < playerInventory.size( ); ++i ) {
		if ( playerInventory[ i ] == -1 ) {
			freeSlot = i;
			break;
		}
	}

	if ( freeSlot == -1 ) {
		Log::Print( "[Shop] Inventory is full." );
		gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_fail_mana" );
		return;
	}

	Globals::Get( ).getGame( )->removeMana( selectedItem->cost );
	playerInventory[ freeSlot ] = selectedItem->id;
	allItemsById[ selectedItem->id ].isOwned = true;

	Log::Print( "[Shop] Item '%s' purchased and equipped in slot %d." , selectedItem->name.c_str( ) , freeSlot );
	gameSoundsEventHandler::Get( ).addEventToQueue( "upgrade_success" );
	recalculatePlayerStats( );
}

void shopHandler::sellItem( int inventorySlotIndex ) {
	std::lock_guard<std::mutex> lock( shopMutex );

	if ( inventorySlotIndex < 0 || inventorySlotIndex >= INVENTORY_SLOTS ) return;

	int itemId = playerInventory[ inventorySlotIndex ];
	if ( itemId == -1 ) {
		Log::Print( "[Shop] Attempted to sell from an empty slot." );
		return;
	}
	int sellValue = static_cast< int >( allItemsById[ itemId ].cost * 0.7f );
	Globals::Get( ).getGame( )->addMana( sellValue );
	allItemsById[ itemId ].isOwned = false;
	playerInventory[ inventorySlotIndex ] = -1;

	Log::Print( "[Shop] Item '%s' sold for %d mana." , allItemsById[ itemId ].name.c_str( ) , sellValue );
	recalculatePlayerStats( );
}

void shopHandler::recalculatePlayerStats( ) {
	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	playerStats * currentStats = Globals::Get( ).getPlayerStats( );

	if ( !player || !currentStats ) {
		Log::Print( "[Shop] Player or stats not found for recalculation." );
		return;
	}

	playerStats baseStats = getBasePlayerStats( );

	// Resetar stats para valores base
	currentStats->setMeleeAttackDamageFactor( baseStats.getMeleeAttackDamageFactor( ) );
	currentStats->setMeleeAttackSpeedFactor( baseStats.getMeleeAttackSpeedFactor( ) );
	currentStats->setMeleeAttackRangeFactor( baseStats.getMeleeAttackRangeFactor( ) );

	currentStats->setRangeAttackDamageFactor( baseStats.getRangeAttackDamageFactor( ) );
	currentStats->setRangeAttackSpeedFactor( baseStats.getRangeAttackSpeedFactor( ) );
	currentStats->setRangeAttackRangeFactor( baseStats.getRangeAttackRangeFactor( ) );

	currentStats->setDefense( baseStats.getDefense( ) );
	currentStats->setMaxHealth( baseStats.getMaxHealth( ) );
	currentStats->setMaxStamina( baseStats.getMaxStamina( ) );
	currentStats->setStaminaLossFactor( baseStats.getStaminaLossFactor( ) );
	currentStats->setStaminaRegenFactor( baseStats.getStaminaRegenFactor( ) );
	currentStats->setMovementSpeed( baseStats.getMovementSpeed( ) );

	// Aplicar bônus de cada item equipado
	for ( int itemId : playerInventory ) {
		if ( itemId != -1 ) {
			const auto & item = allItemsById[ itemId ];
			for ( auto bonus : item.bonuses ) {

				float currentValue = getStatValue( bonus.stat , currentStats );
				if ( currentValue == 0.0f ) {
					Log::Print( "[Shop] Stat %d is zero, skipping bonus application." , static_cast< int >( bonus.stat ) );
					continue;
				}
				// Verifica se o valor do bônus é percentual ou absoluto
				if ( std::fabs( bonus.value ) < 1.0f ) {
					currentValue += ( currentValue * bonus.value );
				}
				else {
					currentValue += bonus.value;
				}

				bonus.value = currentValue; // Atualiza o valor do bônus com o novo valor calculado

				setStatValue( bonus , currentStats );
			}
		}
	}

	currentStats->updatePlayer( );
	Log::Print( "[Shop] Player stats recalculated." );
}

void shopHandler::applyBonusOnStats( const StatBonus & bonus )
{
}

void shopHandler::clearItems( ) {
	std::lock_guard<std::mutex> lock( shopMutex );
	categorizedItems.clear( );
	allItemsById.clear( );
}

void shopHandler::loadInitialItems( ) {
	std::lock_guard<std::mutex> lock( shopMutex );

	// Helper para adicionar itens
	auto addItem = [ & ] ( ItemCategory category , const std::string & name , const std::string & desc , int cost ,
		const std::string & iconName , const std::vector<StatBonus> & bonuses ) {
			void * icon = getIconTexture( iconName );
			categorizedItems[ category ].push_back( { -1, category, name, desc, cost, icon, bonuses } );
		};

	// Starter Items
	addItem( ItemCategory::Starter , "Doran's Blade" , "Um bom item inicial para lutadores." , 100 , "doran_knives" , {
		{PlayerStatIndex::MaxHealth, 10.0f},
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.05f} // 5% de Dano
		} );
	addItem( ItemCategory::Starter , "Boots of Speed" , "Aumenta a velocidade de movimento." , 80 , "speed_boots" , {
		{PlayerStatIndex::MovementSpeed, 15.0f}
		} );
	addItem( ItemCategory::Starter , "Dagger" , "Um pequeno impulso na velocidade de ataque." , 90 , "dagger" , {
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.10f} // 10% de Velocidade de Ataque
		} );
	addItem( ItemCategory::Starter , "Cloth Armor" , "Aumenta a resistência a danos." , 85 , "leather_resistence" , {
		{PlayerStatIndex::Defense, 8.0f}
		} );
	addItem( ItemCategory::Starter , "Tome of Strength" , "Um tomo básico que concede força menor." , 90 , "tome_of_strength" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.04f},
		{PlayerStatIndex::MaxHealth, 5.0f}
		} );
	addItem( ItemCategory::Starter , "Apprentice's Staff" , "Preferido por magos novatos por seu foco." , 90 , "apprentice_staff" , {
		{PlayerStatIndex::RangeAttackDamageFactor, 0.08f} // 8% de Dano à Distância
		} );
	addItem( ItemCategory::Starter , "Hunter's Machete" , "Ideal para ataques rápidos na selva." , 85 , "hunter_machete" , {
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.08f}
		} );
	addItem( ItemCategory::Starter , "Reinforced Boots" , "Um pouco mais pesadas, mas oferecem mais proteção." , 120 , "reinforced_boots" , {
		{PlayerStatIndex::MovementSpeed, 10.0f},
		{PlayerStatIndex::Defense, 4.0f}
		} );
	addItem( ItemCategory::Starter , "Health Pendant" , "Um amuleto simples que aumenta a vitalidade." , 100 , "health_pendant" , {
		{PlayerStatIndex::MaxHealth, 25.0f}
		} );


	// Utility & Enhancement Items
	addItem( ItemCategory::Utility , "Mana Crystal" , "Aumenta a chance de inimigos derrubarem orbes de mana." , 180 , "mana_crystal" , {
	   {PlayerStatIndex::ManaDropProbability, 0.10f} // 10%
		} );
	addItem( ItemCategory::Utility , "Vitality Pendant" , "Aumenta a chance de inimigos derrubarem orbes de vida." , 180 , "vitality_pendant" , {
		{PlayerStatIndex::HealthDropProbability, 0.10f} // 10%
		} );
	addItem( ItemCategory::Utility , "Tome of Energy" , "Concede maior fôlego para suas ações." , 200 , "energy_tome" , {
		{PlayerStatIndex::MaxStamina, 35.0f}
		} );
	addItem( ItemCategory::Utility , "Ring of Persistence" , "Permite que você recupere fôlego mais rapidamente." , 250 , "persistence_ring" , {
		{PlayerStatIndex::StaminaRegen, 0.25f} // 25% de Regeneração de Stamina
		} );
	addItem( ItemCategory::Utility , "Amulet of Wisdom" , "Reduz o custo de fôlego de todas as ações." , 280 , "amulet_of_wisdom" , {
		{PlayerStatIndex::StaminaLoss, -0.15f} // -15% Custo de Stamina
		} );
	addItem( ItemCategory::Utility , "Pathfinder's Compass" , "Faz você se sentir mais leve em seus pés." , 220 , "pathfinder_compass" , {
		{PlayerStatIndex::MovementSpeed, 25.0f}
		} );
	addItem( ItemCategory::Utility , "Chalice of Harmony" , "Um cálice abençoado que atrai energia espiritual." , 350 , "chalice_of_harmony" , {
		{PlayerStatIndex::ManaDropProbability, 0.08f},
		{PlayerStatIndex::StaminaRegen, 0.15f}
		} );
	addItem( ItemCategory::Utility , "Lucky Clover" , "Você se sente um pouco mais sortudo segurando isto." , 320 , "lucky_clover" , {
		{PlayerStatIndex::HealthDropProbability, 0.08f},
		{PlayerStatIndex::ManaDropProbability, 0.08f}
		} );
	addItem( ItemCategory::Utility , "Scout's Goggles" , "Permite uma mira melhor e tiros mais longos." , 300 , "scout_goggles" , {
		{PlayerStatIndex::RangeAttackRangeFactor, 0.12f} // 12% Alcance
		} );


	// Offensive Items
	addItem( ItemCategory::Offensive , "Great Axe" , "Mais pesado e poderoso que uma espada." , 380 , "great_axe" , {
	   {PlayerStatIndex::MeleeAttackDamageFactor, 0.18f} // 18% Dano
		} );
	addItem( ItemCategory::Offensive , "Recurve Bow" , "Oferece um aumento significativo na cadência de tiro." , 420 , "recurve_bow" , {
		{PlayerStatIndex::RangeAttackSpeedFactor, 0.25f} // 25% Velocidade de Ataque à Distância
		} );
	addItem( ItemCategory::Offensive , "Long Spear" , "Permite atacar inimigos de uma distância segura." , 350 , "long_spear" , {
		{PlayerStatIndex::MeleeAttackRangeFactor, 0.25f} // 25% Alcance
		} );
	addItem( ItemCategory::Offensive , "Heavy Flail" , "Aumenta a área de seus golpes, atingindo mais inimigos." , 400 , "heavy_flail" , {
		{PlayerStatIndex::MeleeAttackRangeFactor, 0.20f}
		} );
	addItem( ItemCategory::Offensive , "Warhammer" , "Lento, mas devastador." , 450 , "warhammer" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.25f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, -0.10f} // Penalidade de velocidade
		} );
	addItem( ItemCategory::Offensive , "Phantom Dancer" , "Ataque com a velocidade de um fantasma." , 550 , "phantom_dancer" , {
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.40f}
		} );
	addItem( ItemCategory::Offensive , "Serrated Dirk" , "Uma lâmina cruel que garante cortes profundos." , 420 , "serrated_dirk" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.15f}
		} );
	addItem( ItemCategory::Offensive , "Stormrazor" , "Capacitado pela tempestade, ataca com velocidade relâmpago." , 600 , "stormrazor" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.20f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.20f}
		} );

	// Defensive Items
	addItem( ItemCategory::Defensive , "Tower Shield" , "Um escudo maciço que oferece grande proteção." , 420 , "tower_shield" , {
		{PlayerStatIndex::Defense, 25.0f}
		} );
	addItem( ItemCategory::Defensive , "Giant's Belt" , "Um cinto robusto que aumenta sua vitalidade." , 400 , "giant_belt" , {
		{PlayerStatIndex::MaxHealth, 60.0f}
		} );
	addItem( ItemCategory::Defensive , "Steel Helm" , "Oferece boa proteção para a cabeça e um pouco de vida." , 300 , "steel_helm" , {
		{PlayerStatIndex::Defense, 10.0f},
		{PlayerStatIndex::MaxHealth, 20.0f}
		} );
	addItem( ItemCategory::Defensive , "Guardian's Mail" , "Armadura abençoada por protetores antigos." , 550 , "guardian_mail" , {
		{PlayerStatIndex::Defense, 35.0f}
		} );
	addItem( ItemCategory::Defensive , "Spirit Visage" , "Um elmo que melhora a cura natural." , 580 , "spirit_visage" , {
		{PlayerStatIndex::MaxHealth, 40.0f},
		{PlayerStatIndex::HealthDropProbability, 0.12f}
		} );
	addItem( ItemCategory::Defensive , "Aegis Shield" , "Um escudo equilibrado para defesa versátil." , 500 , "aegis_shield" , {
		{PlayerStatIndex::Defense, 18.0f},
		{PlayerStatIndex::MaxHealth, 25.0f}
		} );
	addItem( ItemCategory::Defensive , "Plated Steelcaps" , "Botas reforçadas com placas de aço." , 480 , "plated_steelcaps" , {
		{PlayerStatIndex::Defense, 20.0f},
		{PlayerStatIndex::MovementSpeed, -5.0f} // Leve penalidade de movimento
		} );
	addItem( ItemCategory::Defensive , "Frozen Heart" , "Um núcleo gelado que torna o portador incrivelmente resistente." , 650 , "frozen_heart" , {
		{PlayerStatIndex::Defense, 45.0f},
		{PlayerStatIndex::MaxStamina, 20.0f}
		} );


	// Hybrid & Advanced Items
	addItem( ItemCategory::Hybrid , "Blade of Fury" , "Uma arma que canaliza fúria em ataques rápidos e mortais." , 750 , "fury_blade" , {
	   {PlayerStatIndex::MeleeAttackDamageFactor, 0.12f},
	   {PlayerStatIndex::MeleeAttackSpeedFactor, 0.25f}
		} );
	addItem( ItemCategory::Hybrid , "Gladiator's Gauntlets" , "Equilibra poder de ataque e resistência." , 700 , "gladiator_gauntlets" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.10f},
		{PlayerStatIndex::Defense, 15.0f}
		} );
	addItem( ItemCategory::Hybrid , "Berserker's Greaves" , "Permite que você se mova rapidamente pelo campo de batalha." , 680 , "berserker_greaves" , {
		{PlayerStatIndex::MovementSpeed, 30.0f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.15f}
		} );
	addItem( ItemCategory::Hybrid , "Sage's Amulet" , "Reduz o custo de fôlego de suas ações." , 650 , "sage_amulet" , {
		{PlayerStatIndex::StaminaLoss, -0.20f},
		{PlayerStatIndex::MaxStamina, 20.0f}
		} );
	addItem( ItemCategory::Hybrid , "Phage" , "Um martelo pesado que o torna mais resistente e mais forte." , 720 , "phage" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.10f},
		{PlayerStatIndex::MaxHealth, 40.0f}
		} );
	addItem( ItemCategory::Hybrid , "Wit's End" , "Drena magia do ar para reforçar as defesas." , 780 , "wits_end" , {
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.20f},
		{PlayerStatIndex::Defense, 20.0f}
		} );
	addItem( ItemCategory::Hybrid , "Sunfire Aegis" , "Uma peça de armadura com um coração em chamas." , 850 , "sunfire_aegis" , {
		{PlayerStatIndex::MaxHealth, 70.0f},
		{PlayerStatIndex::Defense, 25.0f}
		} );
	addItem( ItemCategory::Hybrid , "Rylai's Scepter" , "Um cristal que capacita ataques à distância e vitalidade." , 820 , "rylais_scepter" , {
		{PlayerStatIndex::RangeAttackDamageFactor, 0.15f},
		{PlayerStatIndex::MaxHealth, 50.0f}
		} );
	addItem( ItemCategory::Hybrid , "Black Cleaver" , "Um machado que rasga armaduras e permite perseguir inimigos." , 900 , "black_cleaver" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.15f},
		{PlayerStatIndex::MaxHealth, 30.0f},
		{PlayerStatIndex::MovementSpeed, 10.0f}
		} );


	// Legendary Artifacts
	addItem( ItemCategory::Legendary , "Seismic Warhammer" , "Um martelo colossal que causa dano em uma vasta área." , 1200 , "seismic_warhammer" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.25f},
		{PlayerStatIndex::MeleeAttackRangeFactor, 0.30f},
		{PlayerStatIndex::MaxStamina, 40.0f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, -0.15f}
		} );
	addItem( ItemCategory::Legendary , "Protector's Heart" , "Uma gema que fortalece tanto o corpo quanto a armadura." , 1100 , "protector_heart" , {
		{PlayerStatIndex::MaxHealth, 100.0f},
		{PlayerStatIndex::Defense, 30.0f},
		{PlayerStatIndex::StaminaRegen, 0.15f},
		{PlayerStatIndex::HealthDropProbability, 0.05f}
		} );
	addItem( ItemCategory::Legendary , "Ethereal Whisper" , "Uma relíquia que conecta o portador às energias do mundo." , 1000 , "ethereal_whisper" , {
		{PlayerStatIndex::ManaDropProbability, 0.15f},
		{PlayerStatIndex::StaminaRegen, 0.30f},
		{PlayerStatIndex::StaminaLoss, -0.15f},
		{PlayerStatIndex::MovementSpeed, 20.0f}
		} );
	addItem( ItemCategory::Legendary , "Treasure Hunter's Talisman" , "Aumenta a chance de encontrar todos os tipos de orbes." , 950 , "treasure_hunter_talisman" , {
		{PlayerStatIndex::ManaDropProbability, 0.15f},
		{PlayerStatIndex::HealthDropProbability, 0.15f},
		{PlayerStatIndex::MovementSpeed, 25.0f},
		{PlayerStatIndex::MaxStamina, 25.0f}
		} );
	addItem( ItemCategory::Legendary , "Runic Plate Armor" , "A obra-prima de um ferreiro anão. Quase impenetrável." , 1400 , "runic_plate_armor" , {
		{PlayerStatIndex::Defense, 70.0f},
		{PlayerStatIndex::MaxHealth, 50.0f},
		{PlayerStatIndex::StaminaRegen, 0.20f},
		{PlayerStatIndex::MovementSpeed, -15.0f}
		} );
	addItem( ItemCategory::Legendary , "Trinity Force" , "Três forças combinadas: velocidade, força e vida." , 1500 , "trinity_force" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.18f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.18f},
		{PlayerStatIndex::MaxHealth, 40.0f},
		{PlayerStatIndex::MovementSpeed, 20.0f}
		} );
	addItem( ItemCategory::Legendary , "Bloodthirster" , "Uma lâmina viva que anseia por batalha." , 1350 , "bloodthirster" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.35f},
		{PlayerStatIndex::MaxHealth, 40.0f},
		{PlayerStatIndex::HealthDropProbability, 0.10f}, // Simula "roubo de vida"
		{PlayerStatIndex::Defense, 10.0f}
		} );
	addItem( ItemCategory::Legendary , "Infinity Edge" , "Uma lâmina que parece cortar a própria realidade." , 1600 , "infinity_edge" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.40f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.25f},
		{PlayerStatIndex::MeleeAttackRangeFactor, 0.15f},
		{PlayerStatIndex::Defense, -10.0f} // Alto risco, alta recompensa
		} );
	addItem( ItemCategory::Legendary , "Rabadon's Deathcap" , "O chapéu favorito do arquimago Rabadon. Poder imenso." , 1600 , "rabadons_deathcap" , {
		{PlayerStatIndex::RangeAttackDamageFactor, 0.45f},
		{PlayerStatIndex::RangeAttackRangeFactor, 0.20f},
		{PlayerStatIndex::MaxStamina, 50.0f},
		{PlayerStatIndex::ManaDropProbability, 0.10f}
		} );
	addItem( ItemCategory::Legendary , "Blade of the Ruined King" , "Uma lâmina amaldiçoada com vontade própria." , 1450 , "bork" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.25f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.35f},
		{PlayerStatIndex::MovementSpeed, 25.0f},
		{PlayerStatIndex::MaxHealth, 30.0f}
		} );

	// -- Artefatos Míticos (Preços Extremos) --
	// Itens que alteram fundamentalmente o jogo. Extremamente raros e caros.
	addItem( ItemCategory::Legendary , "Essence of the Void" , "Um poder corruptor que destrói a defesa inimiga, mas torna o portador frágil." , 2500 , "essence_of_the_void" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.50f},
		{PlayerStatIndex::RangeAttackDamageFactor, 0.50f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.20f},
		{PlayerStatIndex::RangeAttackSpeedFactor, 0.20f},
		{PlayerStatIndex::Defense, -30.0f} // Penalidade severa
		} );
	addItem( ItemCategory::Legendary , "Shard of Creation" , "Um eco da forja do mundo. Transborda com energia primordial." , 2800 , "shard_of_creation" , {
		{PlayerStatIndex::StaminaRegen, 0.50f},
		{PlayerStatIndex::StaminaLoss, -0.30f},
		{PlayerStatIndex::HealthDropProbability, 0.25f},
		{PlayerStatIndex::ManaDropProbability, 0.25f}
		} );
	addItem( ItemCategory::Legendary , "Echo of a Thousand Battles" , "A habilidade de inúmeros guerreiros lendários guia sua mão." , 4000 , "echo_of_a_thousand_battles" , {
		{PlayerStatIndex::MeleeAttackDamageFactor, 0.30f},
		{PlayerStatIndex::MeleeAttackSpeedFactor, 0.30f},
		{PlayerStatIndex::Defense, 30.0f},
		{PlayerStatIndex::MaxHealth, 80.0f},
		{PlayerStatIndex::MovementSpeed, 30.0f},
		{PlayerStatIndex::StaminaRegen, 0.30f}
		} );

	// Atribui IDs únicos
	int idCounter = 0;
	for ( int i = 0; i < ( int ) ItemCategory::Legendary + 1; ++i ) {
		ItemCategory cat = static_cast< ItemCategory >( i );
		if ( categorizedItems.count( cat ) ) {
			for ( auto & item : categorizedItems.at( cat ) ) {
				item.id = idCounter++;
				allItemsById.push_back( item );
			}
		}
	}

	recalculatePlayerStats( );
}
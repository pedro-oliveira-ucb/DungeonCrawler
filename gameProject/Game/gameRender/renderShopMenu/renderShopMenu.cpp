#include "renderShopMenu.h"
#include "../../Handlers/shopHandler/shopHandler.h"
#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

// Função auxiliar para texto centralizado
void DrawTextCentered( const char * text , Rectangle container , int fontSize , Color color ) {
	Vector2 textSize = MeasureTextEx( GetFontDefault( ) , text , fontSize , 1 );
	DrawTextEx( GetFontDefault( ) , text ,
		{ container.x + ( container.width - textSize.x ) / 2, container.y + ( container.height - textSize.y ) / 2 } ,
		fontSize , 1 , color );
}

void renderShopMenu::initializeLayout( ) {
	float screenW = GetScreenWidth( );
	float screenH = GetScreenHeight( );

	// Configuração dos paineis principais
	shopPanel = { screenW * 0.1f, screenH * 0.1f, screenW * 0.8f, screenH * 0.8f };
	detailsPanel = { shopPanel.x + shopPanel.width * 0.7f, shopPanel.y, shopPanel.width * 0.3f, shopPanel.height };
	itemsGridPanel = { shopPanel.x, shopPanel.y, shopPanel.width * 0.7f, shopPanel.height };

	// Configuração das abas de categorias
	categoryTabsRects.clear( );
	const auto & categoryNames = shopHandler::Get( ).getCategoryNames( );
	float tabWidth = itemsGridPanel.width / categoryNames.size( );
	float tabHeight = 40.0f;
	float currentX = itemsGridPanel.x;

	for ( int i = 0; i <= ( int ) ItemCategory::Legendary; ++i ) {
		ItemCategory cat = static_cast< ItemCategory >( i );
		if ( categoryNames.count( cat ) ) {
			categoryTabsRects[ cat ] = { currentX, itemsGridPanel.y + 50, tabWidth, tabHeight };
			currentX += tabWidth;
		}
	}

	// Configuração dos slots de inventário
	inventorySlotsRects.clear( );
	const int INVENTORY_SLOTS = 6;
	const float invSlotSize = 70;
	const float padding = 15;
	float totalInvWidth = INVENTORY_SLOTS * ( invSlotSize + padding );
	float invStartX = shopPanel.x + ( shopPanel.width - totalInvWidth ) / 2.0f;

	for ( int i = 0; i < INVENTORY_SLOTS; ++i ) {
		inventorySlotsRects.push_back( {
			invStartX + i * ( invSlotSize + padding ),
			shopPanel.y + shopPanel.height - invSlotSize - padding,
			invSlotSize,
			invSlotSize
			} );
	}

	// Atualiza layout dos itens baseado na categoria atual
	updateItemLayout( );

	isInitialized = true;
}

void renderShopMenu::updateItemLayout( ) {
	itemSlotsRects.clear( );

	ItemCategory selectedCategory = shopHandler::Get( ).getSelectedCategory( );
	const auto & categorizedItems = shopHandler::Get( ).getCategorizedItems( );

	if ( categorizedItems.count( selectedCategory ) ) {
		const int cols = 5;
		const float slotSize = 90;
		const float padding = 15;
		float startY = itemsGridPanel.y + 100; // Começa a desenhar itens abaixo das abas de categoria

		const auto & itemsInCategory = categorizedItems.at( selectedCategory );
		for ( size_t i = 0; i < itemsInCategory.size( ); ++i ) {
			int row = i / cols;
			int col = i % cols;
			itemSlotsRects.push_back( {
				itemsGridPanel.x + padding + col * ( slotSize + padding ),
				startY + row * ( slotSize + padding ),
				slotSize,
				slotSize
				} );
		}
	}
}

void renderShopMenu::handleMouseInput( ) {
	Vector2 mousePos = GetMousePosition( );
	shopHandler & shop = shopHandler::Get( );

	// Reset do item selecionado
	shop.setSelectedItem( nullptr );

	if ( IsMouseButtonPressed( MOUSE_LEFT_BUTTON ) ) {
		// Verifica clique nas abas de categoria
		bool tabClicked = false;
		for ( const auto & pair : categoryTabsRects ) {
			if ( CheckCollisionPointRec( mousePos , pair.second ) ) {
				if ( shop.getSelectedCategory( ) != pair.first ) {
					shop.setSelectedCategory( pair.first );
					updateItemLayout( );
				}
				tabClicked = true;
				break;
			}
		}

		// Se não clicou em uma aba, verifica compra de item
		if ( !tabClicked ) {
			const auto & categorizedItems = shop.getCategorizedItems( );
			ItemCategory selectedCategory = shop.getSelectedCategory( );

			if ( categorizedItems.count( selectedCategory ) ) {
				const auto & currentCategoryItems = categorizedItems.at( selectedCategory );
				for ( size_t i = 0; i < currentCategoryItems.size( ) && i < itemSlotsRects.size( ); ++i ) {
					if ( CheckCollisionPointRec( mousePos , itemSlotsRects[ i ] ) ) {
						int itemId = currentCategoryItems[ i ].id;
						const auto & allItems = shop.getAllItems( );
						shop.setSelectedItem( const_cast< ShopItem * >( &allItems[ itemId ] ) );
						shop.buySelectedItem( );
						break;
					}
				}
			}
		}
	}

	// Verificar hover e venda
	const auto & allItems = shop.getAllItems( );
	const auto & playerInventory = shop.getPlayerInventory( );

	// Primeiro, verificar itens de inventário
	for ( int i = 0; i < playerInventory.size( ) && i < inventorySlotsRects.size( ); ++i ) {
		if ( CheckCollisionPointRec( mousePos , inventorySlotsRects[ i ] ) && playerInventory[ i ] != -1 ) {
			shop.setSelectedItem( const_cast< ShopItem * >( &allItems[ playerInventory[ i ] ] ) );
			if ( IsMouseButtonPressed( MOUSE_RIGHT_BUTTON ) ) {
				shop.sellItem( i );
			}
			return; // Priorizar seleção de inventário
		}
	}

	// Depois, verificar itens da loja na categoria atual
	ItemCategory selectedCategory = shop.getSelectedCategory( );
	const auto & categorizedItems = shop.getCategorizedItems( );

	if ( categorizedItems.count( selectedCategory ) ) {
		const auto & currentCategoryItems = categorizedItems.at( selectedCategory );
		for ( size_t i = 0; i < currentCategoryItems.size( ) && i < itemSlotsRects.size( ); ++i ) {
			if ( CheckCollisionPointRec( mousePos , itemSlotsRects[ i ] ) ) {
				int itemId = currentCategoryItems[ i ].id;
				shop.setSelectedItem( const_cast< ShopItem * >( &allItems[ itemId ] ) );
				break;
			}
		}
	}
}

void renderShopMenu::renderCategoryTabs( ) {
	const auto & categoryNames = shopHandler::Get( ).getCategoryNames( );
	ItemCategory selectedCategory = shopHandler::Get( ).getSelectedCategory( );

	for ( const auto & pair : categoryTabsRects ) {
		Color tabColor = ( selectedCategory == pair.first ) ? Color { 40, 42, 52, 255 } : Color { 20, 22, 32, 255 };
		Color textColor = ( selectedCategory == pair.first ) ? GOLD : WHITE;

		DrawRectangleRec( pair.second , tabColor );
		DrawRectangleLinesEx( pair.second , 2 , { 80, 88, 128, 255 } );
		DrawTextCentered( categoryNames.at( pair.first ).c_str( ) , pair.second , 18 , textColor );
	}
}

void renderShopMenu::renderItemGrid( ) {
	const auto & categorizedItems = shopHandler::Get( ).getCategorizedItems( );
	ItemCategory selectedCategory = shopHandler::Get( ).getSelectedCategory( );
	ShopItem * selectedItem = shopHandler::Get( ).getSelectedItem( );

	if ( categorizedItems.count( selectedCategory ) && !itemSlotsRects.empty( ) ) {
		const auto & currentCategoryItems = categorizedItems.at( selectedCategory );
		const auto & allItems = shopHandler::Get( ).getAllItems( );

		for ( size_t i = 0; i < currentCategoryItems.size( ) && i < itemSlotsRects.size( ); ++i ) {
			const auto & displayItem = currentCategoryItems[ i ];
			// Obter estado atual do item master
			const auto & dataItem = allItems[ displayItem.id ];

			Rectangle slotRect = itemSlotsRects[ i ];

			Color borderColor = DARKGRAY;
			if ( selectedItem && selectedItem->id == dataItem.id ) {
				borderColor = GOLD;
			}
			else if ( dataItem.isOwned ) {
				borderColor = RED;
			}

			DrawRectangleRec( slotRect , { 10, 11, 16, 255 } );

			if ( displayItem.icon != nullptr ) {
				Texture2D * icon = static_cast< Texture2D * >( displayItem.icon );
				Rectangle destRect = { slotRect.x + 5, slotRect.y + 5, slotRect.width - 10, slotRect.height - 30 };
				Rectangle sourceRect = { 0, 0, ( float ) icon->width, ( float ) icon->height };
				DrawTexturePro( *icon , sourceRect , destRect , { 0, 0 } , 0.0f , WHITE );
			}

			DrawTextCentered( std::to_string( displayItem.cost ).c_str( ) ,
				{ slotRect.x, slotRect.y + slotRect.height - 25, slotRect.width, 20 } , 20 , GOLD );
			DrawRectangleLinesEx( slotRect , 2 , borderColor );
		}
	}
}

std::string statToText( PlayerStatIndex index ) {


	switch ( index ) {
	case PlayerStatIndex::MaxHealth:
		return "Max health";
	case PlayerStatIndex::MaxStamina:
		return "Max stamina";
	case PlayerStatIndex::StaminaRegen:
		return "Stamina regeneration";
	case PlayerStatIndex::StaminaLoss:
		return "Stamina cost";
	case PlayerStatIndex::MeleeAttackDamageFactor:
		return "Melee attack damage";
	case PlayerStatIndex::MeleeAttackSpeedFactor:
		return "Melee attack speed";
	case PlayerStatIndex::MeleeAttackRangeFactor:
		return "Melee attack range";
	case PlayerStatIndex::RangeAttackDamageFactor:
		return "Range attack damage";
	case PlayerStatIndex::RangeAttackSpeedFactor:
		return "Range attack speed";
	case PlayerStatIndex::RangeAttackRangeFactor:
		return "Range attack range";
	case PlayerStatIndex::Defense:
		return "Defense";
	case PlayerStatIndex::MovementSpeed:
		return "Movement speed";
	case PlayerStatIndex::ManaDropProbability:
		return "Mana drop probability";
	case PlayerStatIndex::HealthDropProbability:
		return "Health drop probability";
	}

	return "";
}

void renderShopMenu::renderDetailsPanel( ) {
	DrawRectangleRec( detailsPanel , { 10, 11, 16, 255 } );
	DrawRectangleLinesEx( detailsPanel , 2 , { 80, 88, 128, 255 } );

	ShopItem * selectedItem = shopHandler::Get( ).getSelectedItem( );

	if ( selectedItem ) {
		DrawText( selectedItem->name.c_str( ) , detailsPanel.x + 15 , detailsPanel.y + 20 , 20 , GOLD );

		if ( selectedItem->icon != nullptr ) {
			Texture2D * icon = static_cast< Texture2D * >( selectedItem->icon );
			Rectangle destRect = { detailsPanel.x + 15, detailsPanel.y + 50, 70, 70 };
			Rectangle sourceRect = { 0, 0, ( float ) icon->width, ( float ) icon->height };
			DrawTexturePro( *icon , sourceRect , destRect , { 0, 0 } , 0.0f , WHITE );
		}

		std::string costText = "Cost: " + std::to_string( selectedItem->cost );
		DrawText( costText.c_str( ) , detailsPanel.x + 95 , detailsPanel.y + 55 , 20 , WHITE );

		if ( selectedItem->isOwned ) {
			DrawText( "(Owned)" , detailsPanel.x + 95 , detailsPanel.y + 80 , 20 , RED );
		}

		// Renderizar bônus de status
		float currentY_details = detailsPanel.y + 140;
		for ( const auto & bonus : selectedItem->bonuses ) {
			std::string text;

			if ( bonus.value > 0.0f )
				text += "+";
			else
				text += "-";

			if ( std::fabs( bonus.value ) < 1.0f ) {
				// percentage
				text += std::to_string( int( std::fabs( bonus.value ) * 100.0f ) );
				text += "%";
			}
			else
				text += std::to_string( ( int ) ( bonus.value ) );

			text += " " + statToText( bonus.stat );


			DrawText( text.c_str() , detailsPanel.x + 15 , currentY_details , 18 , SKYBLUE );
			currentY_details += 25;
		}

		// Renderizar descrição
		DrawTextEx( GetFontDefault( ) , selectedItem->description.c_str( ) ,
			{ detailsPanel.x + 15, currentY_details + 10 } , 16 , 1.0f , WHITE );
	}
	else {
		DrawTextCentered( "Hover over an item\nto see details." , detailsPanel , 20 , GRAY );
	}
}

void renderShopMenu::renderInventory( ) {
	// Título do inventário
	DrawTextCentered( "Inventory (Right-click to sell)" ,
		{ shopPanel.x, shopPanel.y + shopPanel.height - 120, shopPanel.width, 20 } , 20 , WHITE );

	// Slots de inventário
	const auto & playerInventory = shopHandler::Get( ).getPlayerInventory( );
	const auto & allItems = shopHandler::Get( ).getAllItems( );
	ShopItem * selectedItem = shopHandler::Get( ).getSelectedItem( );

	for ( int i = 0; i < playerInventory.size( ) && i < inventorySlotsRects.size( ); ++i ) {
		Rectangle invRect = inventorySlotsRects[ i ];
		Color borderColor = DARKGRAY;

		DrawRectangleRec( invRect , { 10, 11, 16, 255 } );

		if ( playerInventory[ i ] != -1 ) {
			const auto & item = allItems[ playerInventory[ i ] ];
			if ( item.icon != nullptr ) {
				Texture2D * icon = static_cast< Texture2D * >( item.icon );
				Rectangle sourceRect = { 0, 0, ( float ) icon->width, ( float ) icon->height };
				Rectangle destRect = { invRect.x + 2, invRect.y + 2, invRect.width - 4, invRect.height - 4 };
				DrawTexturePro( *icon , sourceRect , destRect , { 0, 0 } , 0.0f , WHITE );
			}

			if ( selectedItem && selectedItem->id == item.id ) {
				borderColor = GOLD;
			}
		}

		DrawRectangleLinesEx( invRect , 2 , borderColor );
	}
}

void renderShopMenu::render( ) {
	if ( !isInitialized || IsWindowResized( ) ) {
		initializeLayout( );
	}

	// Processar input do mouse
	handleMouseInput( );

	// Renderização do fundo
	DrawRectangle( 0 , 0 , GetScreenWidth( ) , GetScreenHeight( ) , Fade( BLACK , 0.8f ) );
	DrawRectangleRec( shopPanel , { 20, 22, 32, 255 } );
	DrawRectangleLinesEx( shopPanel , 2 , { 80, 88, 128, 255 } );

	// Título e informações da loja
	DrawTextCentered( "Item Shop" , { shopPanel.x, shopPanel.y + 10, itemsGridPanel.width, 40 } , 40 , GOLD );
	std::string manaText = "Mana: " + std::to_string( Globals::Get( ).getGame( )->getNumMana( ) );
	DrawText( manaText.c_str( ) , shopPanel.x + 20 , shopPanel.y + 25 , 20 , YELLOW );

	// Renderizar componentes
	renderCategoryTabs( );
	renderItemGrid( );
	renderDetailsPanel( );
	renderInventory( );

	// Dica de fechamento
	const char * closeHint = "Press [U] or [ESC] to close";
	Vector2 closeHintSize = MeasureTextEx( GetFontDefault( ) , closeHint , 20 , 1 );
	DrawTextEx( GetFontDefault( ) , closeHint ,
		{ GetScreenWidth( ) / 2.0f - closeHintSize.x / 2.0f, shopPanel.y + shopPanel.height + 20 } ,
		20 , 1 , WHITE );
}
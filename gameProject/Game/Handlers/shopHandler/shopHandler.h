#pragma once

#include "../../../Utils/singleton.h"
#include "../../../Globals/playerStats/playerStats.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <mutex>

enum class ItemCategory {
	Starter ,
	Utility ,
	Offensive ,
	Defensive ,
	Hybrid ,
	Legendary
};

// Estrutura para b�nus de status
struct StatBonus {
	PlayerStatIndex stat;
	float value;
};

// Estrutura para item da loja
struct ShopItem {
	int id;
	ItemCategory category;
	std::string name;
	std::string description;
	int cost;
	void * icon; // Ponteiro para Texture2D
	std::vector<StatBonus> bonuses;
	bool isOwned = false;
};

// Gerenciador principal da loja
class shopHandler : public CSingleton<shopHandler> {
private:
	std::mutex shopMutex;

	// "Banco de dados" de itens dispon�veis
	std::map<ItemCategory , std::vector<ShopItem>> categorizedItems;
	std::vector<ShopItem> allItemsById;

	// Mapeamento de nomes de categorias para exibi��o
	std::map<ItemCategory , std::string> categoryNames;

	// Invent�rio do jogador (armazena IDs de itens, -1 para slot vazio)
	const int INVENTORY_SLOTS = 6;
	std::vector<int> playerInventory;

	// Item atualmente selecionado
	ShopItem * selectedItem = nullptr;

	// Categoria atualmente selecionada
	ItemCategory selectedCategory = ItemCategory::Starter;

	// Layouts da UI (para uso visual)
	std::map<std::string , float> layoutInfo;

	void recalculatePlayerStats( );
	void applyBonusOnStats( const StatBonus & bonus );

public:
	shopHandler( );

	// M�todos de acesso aos dados
	ShopItem * getSelectedItem( ) const;
	void setSelectedItem( ShopItem * item );
	ItemCategory getSelectedCategory( ) const;
	void setSelectedCategory( ItemCategory category );
	const std::map<ItemCategory , std::string> & getCategoryNames( ) const;
	const std::map<ItemCategory , std::vector<ShopItem>> & getCategorizedItems( ) const;
	const std::vector<ShopItem> & getAllItems( ) const;
	const std::vector<int> & getPlayerInventory( ) const;

	// M�todos de l�gica de neg�cio
	void buySelectedItem( );
	void sellItem( int inventorySlotIndex );


	// M�todos de inicializa��o
	void clearItems( );
	void loadInitialItems( );
};
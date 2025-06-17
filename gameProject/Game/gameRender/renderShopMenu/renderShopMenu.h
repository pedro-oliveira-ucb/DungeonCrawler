	#pragma once
#include <raylib/raylib.h>
#include <map>
#include <vector>
#include "../../Handlers/shopHandler/shopHandler.h"

class renderShopMenu {
private:
    // Retângulos para o layout da UI
    Rectangle shopPanel;
    Rectangle detailsPanel;
    Rectangle itemsGridPanel;
    std::vector<Rectangle> itemSlotsRects;
    std::vector<Rectangle> inventorySlotsRects;
    std::map<ItemCategory, Rectangle> categoryTabsRects;
    
    bool isInitialized = false;
    
    // Métodos internos de renderização
    void initializeLayout();
    void updateItemLayout();
    void renderCategoryTabs();
    void renderItemGrid();
    void renderDetailsPanel();
    void renderInventory();
    void handleMouseInput();

public:
    void render();
};


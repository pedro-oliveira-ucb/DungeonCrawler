#include "ShopInitializer.h"
#include "../../../Handlers/shopHandler/shopHandler.h"
#include "../../../../Utils/Log/Log.h"

bool ShopInitializer::initialize() {
    Log::Print("[ShopInitializer] Initializing shop system...");
    
    // Limpa itens existentes (caso haja) e carrega os itens iniciais
    shopHandler::Get().clearItems();
    shopHandler::Get().loadInitialItems();
    
    Log::Print("[ShopInitializer] Shop system initialized successfully.");
    return true;
}
#pragma once
#include "../../../../Utils/singleton.h"
#include <string>

class ShopInitializer : public CSingleton<ShopInitializer> {
public:
    bool initialize();
};
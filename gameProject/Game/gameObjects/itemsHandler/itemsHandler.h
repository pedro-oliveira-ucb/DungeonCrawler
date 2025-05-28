#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "../../../Utils/singleton.h"
#include "../../SDK/Entities/Items/CBaseItem/CBaseItem.h"

class itemsHandler : public CSingleton<itemsHandler>
{
	std::mutex handlerMutex;
	std::unordered_map<ItemType , std::unique_ptr<CBaseItem>> spawnableItems;
	std::vector<std::unique_ptr<CBaseItem>> spawnedItems;

public:
	void updateItems( );


	void addSpawnableItem( std::unique_ptr<CBaseItem> item );
	std::unordered_map<ItemType , std::unique_ptr<CBaseItem>> * getSpawnableItems( );

	void spawnItem( std::unique_ptr<CBaseItem> item, GVector2D position);
	std::vector<std::unique_ptr<CBaseItem>> * getSpawnedItems( );
};


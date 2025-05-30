#pragma once
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
	std::unique_ptr<CBaseItem> * getSpawnableItem( ItemType type );

	void spawnItem( ItemType type );

	std::vector<std::unique_ptr<CBaseItem>> * getSpawnedItems( );
};


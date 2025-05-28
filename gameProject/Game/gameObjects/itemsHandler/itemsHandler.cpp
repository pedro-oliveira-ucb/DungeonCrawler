#include "itemsHandler.h"

#include "../entitiesHandler/entitiesHandler.h"

#include "../../Managers/collisionManager/collisionManager.h"

void itemsHandler::updateItems( ) {

	std::lock_guard<std::mutex> lock( handlerMutex );
	CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );
	if ( localPlayer == nullptr )
		return;

	if ( this->spawnableItems.empty( ) ) {
		return;
	}

    auto it = spawnedItems.begin( );
    while ( it != spawnedItems.end( ) ) {
        CBaseItem * item = it->get( );
        if ( !item || !item->getEntityAnimations( ) ) {
            it = spawnedItems.erase( it );
            continue;
        }

        if ( item->isActive( ) ) {
            if ( CollisionManager::Get( ).checkCollision( item , localPlayer , item->getEntityPosition( ) ) ) {
                item->applyEffect( );
                item->Deactive( );
                // não removemos aqui, esperamos que a próxima iteração remova
                ++it;
                continue;
            }
        }
        else {
            it = spawnedItems.erase( it );
            continue;
        }

        item->getEntityAnimations( )->updateAnimation( );
        ++it;
    }

}

std::vector<std::unique_ptr<CBaseItem>> * itemsHandler::getSpawnedItems( )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	return &spawnedItems;
}

void itemsHandler::spawnItem( std::unique_ptr<CBaseItem> item, GVector2D position)
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	item->Active( position );
	spawnedItems.push_back( std::move( item ) );
}

void itemsHandler::addSpawnableItem( std::unique_ptr<CBaseItem> item )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	spawnableItems.emplace( item->getItemType( ) , std::move( item ) );
}

std::unordered_map<ItemType , std::unique_ptr<CBaseItem>> * itemsHandler::getSpawnableItems( )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	return &spawnableItems;
}
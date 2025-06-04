#include "gameMap.h"
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"

void gameMap::init( )
{
	setMap( firstMap );
}

void gameMap::setMap( mapType type )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	this->currentMapType = type;
	this->currentTileSet = *mapObjectsHandler::Get( ).getTileSet( type );
	this->currentMapObjects = mapObjectsHandler::Get( ).getAllMapObjects( type );
	this->mapSize = this->calculateTileSize( );
}

GVector2D gameMap::calculateTileSize( ) {
	GVector2D tilesize( 0 , 0 );

	// Check if the current tile set and map objects are valid
	if ( this->currentTileSet.empty( ) || this->currentMapObjects->empty( ) ) {
		return tilesize;
	}

	TileSet & tileSet = this->currentTileSet;
	auto & tileObjects = this->currentMapObjects;

	float totalWidth = 0.0f;
	float totalHeight = 0.0f;

	float columnWidth = 0.0f;
	float rowHeight = 0.0f;

	for ( int columnIdx = 0; columnIdx < tileSet.size( ); columnIdx++ ) {
		if ( tileSet.at( columnIdx ).empty( ) ) {
			continue;
		}

		auto & column = tileSet.at( columnIdx );

		for ( int lineIdx = 0; lineIdx < tileSet.at( columnIdx ).size( ); lineIdx++ ) {

			auto & line = column.at( lineIdx );

			if ( tileObjects->find( line ) == tileObjects->end( ) ) {
				continue; // Skip if the tile object is not found
			}

			GVector2D spriteSize = tileObjects->at( line )->getSprite( )->get( )->getSpriteSize( );

			if ( !columnIdx ) {
				if ( !columnWidth && spriteSize.x ) {
					columnWidth = spriteSize.x;
				}
			}

			if ( !lineIdx ) {
				if ( !rowHeight && spriteSize.y ) {
					rowHeight = spriteSize.y;
				}
			}	
		}

		totalHeight += rowHeight;
	}

	totalWidth = tileSet.at( 0 ).size( ) * columnWidth;

	return GVector2D( totalWidth , totalHeight );
}

GVector2D gameMap::getMapSize( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->mapSize;
}

mapType gameMap::getCurrentMapType( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->currentMapType;
}

TileSet gameMap::getCurrentTileSet( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->currentTileSet;
}

std::unordered_map<mapObjectType , std::shared_ptr<rMapObject>> * gameMap::getCurrentMapObjects( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->currentMapObjects;
}

GVector2D gameMap::getMapPosition( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->mapPosition;
}

bool gameMap::inInMap( GVector2D position ) 
{
	std::lock_guard<std::mutex> lock( this->mapMutex );

	GVector2D minMapPosition = this->mapPosition;
	GVector2D maxMapPosition = this->mapPosition + this->mapSize;

	// Check if the position is within the bounds of the map
	if(position.x > maxMapPosition.x || position.x < minMapPosition.x ) {
		return false; 
	}	

	if(position.y > maxMapPosition.y || position.y < minMapPosition.y ) {
		return false; 
	}	

	// check if its ground
	if ( this->currentTileSet.empty( ) || this->currentMapObjects->empty( ) ) {
		return false;
	}

	TileSet & tileSet = this->currentTileSet;
	auto & tileObjects = this->currentMapObjects;

	GVector2D mapTileSections = ( this->mapSize ) / GVector2D( (float)this->currentTileSet.at( 0 ).size( ), (float)this->currentTileSet.size( ));
	GVector2D tilePosition = ( position - this->mapPosition ) / mapTileSections;

	int columnIdx = static_cast< int >( tilePosition.y  );
	int lineIdx = static_cast< int >( tilePosition.x  );
	if ( columnIdx < 0 || columnIdx >= tileSet.size( ) || lineIdx < 0 || lineIdx >= tileSet.at( columnIdx ).size( ) ) {
		return false; // Out of bounds
	}

	auto & tileLine = tileSet.at( columnIdx ).at( lineIdx );
	if ( tileObjects->find( tileLine ) != tileObjects->end( ) && tileLine == mapObjectType::ground) {
		return true; // Tile is valid and exists in the map objects
	}

	return false;
}

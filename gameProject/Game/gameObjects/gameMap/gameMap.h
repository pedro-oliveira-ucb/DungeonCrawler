#pragma once
#include <unordered_map>
#include <mutex>
#include <map>
#include <vector> // Required for std::vector
#include <memory> // Required for std::shared_ptr

#include "../../../Utils/singleton.h" // Assuming path is correct
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h" // Assuming path is correct
#include "../../SDK/math/Vector2D/GVector2D.h" // Assuming path is correct

// Forward declaration if rMapObject is defined elsewhere
// class rMapObject; 
// enum class mapObjectType; // Assuming mapObjectType is defined, possibly in mapObjectsHandler.h
// using TileSet = std::vector<std::vector<mapObjectType>>; // Assuming TileSet is defined

struct DoorInstanceData {
	bool unlocked = false;
	// Potentially other door-specific data in the future
	// int leadsToRoomId = 0; // Example: if doors explicitly connect known rooms

	DoorInstanceData( bool unlocked = false )
		: unlocked( unlocked ) {
	}
};

class gameMap : public CSingleton<gameMap>
{
private:
	mutable std::mutex mapMutex;
	std::map<GVector2D , DoorInstanceData> doorInstances;
	mapType currentMapType = firstMap;
	TileSet currentTileSet; // std::vector<std::vector<mapObjectType>>
	std::unordered_map < mapObjectType , std::shared_ptr<rMapObject>> * currentMapObjects;
	GVector2D mapPosition = GVector2D( 0 , 0 ); // Top-left world position of the map
	GVector2D mapSize = GVector2D( 0 , 0 );     // Total world dimensions of the map
	GVector2D tileDimensions = GVector2D( 0 , 0 ); // Dimensions of a single tile

	// Room identification members
	std::vector<std::vector<int>> roomGrid; // Stores room ID for each tile. 0 = no room/wall, >0 = room ID.
	int nextRoomId;                         // Counter for assigning new room IDs.

	GVector2D calculateTileSize( );
	void identifyRooms( ); // Method to run BFS/DFS to find and ID rooms
	bool isTileWalkable( int tileX , int tileY ); // Helper to check if a tile at (tileX, tileY) is part of a room's traversable area
	GVector2D worldToTileCoordinates( GVector2D worldPos ) const; // Converts world coordinates to tile grid indices
	GVector2D tileToWorldCoordinates( GVector2D tilePos ) const;

public:
	void init( );
	// Load the map
	void setMap( mapType type );
	mapType getCurrentMapType( ) const;
	TileSet getCurrentTileSet( ) const;
	// Get the current map objects
	std::unordered_map < mapObjectType , std::shared_ptr<rMapObject>> * getCurrentMapObjects( ) const;
	GVector2D getMapPosition( ) const;
	GVector2D getMapSize( ) const;
	GVector2D getTileDimensions( ) const; // Getter for tile dimensions

	bool inInMap( GVector2D position ); // Checks if a world position is within map boundaries and on a ground tile

	// Door related methods
	bool isDoorUnlocked( GVector2D position );
	void setDoorUnlocked( GVector2D position , bool unlocked );
	DoorInstanceData * getDoorInstanceData( GVector2D position );
	std::map<GVector2D , DoorInstanceData> getDoorInstancesCopy( );

	// Room related methods
	int getRoomIdAtPosition( GVector2D worldPos ); // Returns the room ID for a given world position
	bool areInSameRoom( GVector2D worldPos1 , GVector2D worldPos2 ); // Checks if two world positions are in the same room
	GVector2D getRandomPlace( int roomID );
	bool isCorridor( GVector2D worldPos );
	int getNumRooms( ) const; // Novo método
};

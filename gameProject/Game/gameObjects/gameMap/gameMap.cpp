#include "gameMap.h"
#include <queue> // For std::queue in BFS room identification
#include <random>

#include "../../../Utils/Log/Log.h"

#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h" // Assuming path is correct
#include "../../Handlers/entitiesHandler/entitiesHandler.h"

void gameMap::init( )
{
	setMap( firstMap );
}

void gameMap::setMap( mapType type )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	this->currentMapType = type;

	// Ensure mapObjectsHandler is valid and getTileSet returns a valid pointer
	mapObjectsHandler & handler = mapObjectsHandler::Get( );
	DungeonLayout * layout = handler.getDungeonLayout( type );

	const TileSet tileSetPtr = layout->tiles;
	if ( !tileSetPtr.empty() ) {
		this->currentTileSet = tileSetPtr; // Create a local copy
	}
	else {
		this->currentTileSet.clear( );
		// Handle error: tile set not found for the given type
		// You might want to log this or throw an exception
	}

	this->currentMapObjects = handler.getAllMapObjects( type );
	if ( !this->currentMapObjects ) {
		// Handle error: map objects not found
		// You might want to log this or throw an exception
		// For now, clear relevant data to prevent crashes
		this->currentTileSet.clear( );
	}

	this->mapSize = this->calculateTileSize( ); // This will also set this->tileDimensions

	// Proceed with room identification if the map and tiles are valid
	if ( !this->currentTileSet.empty( ) &&
		!this->currentTileSet[ 0 ].empty( ) &&
		this->tileDimensions.x > 0 &&
		this->tileDimensions.y > 0 ) {
		this->identifyRooms( );
	}
	else {
		this->roomGrid.clear( );
		this->nextRoomId = 0; // Reset room ID counter
		if ( this->tileDimensions.x <= 0 || this->tileDimensions.y <= 0 ) {
			// Log error: tile dimensions could not be determined
		}
	}

	GVector2D spawnPoint = layout->spawnPos;
	spawnPoint = spawnPoint * this->tileDimensions;

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	if ( player != nullptr ) {
		player->setEntityPosition( spawnPoint );
	}
}

// Calculates total map size and individual tile dimensions.
// Also updates door instance positions based on the grid.
GVector2D gameMap::calculateTileSize( ) {
	GVector2D totalMapSize( 0 , 0 );
	this->tileDimensions = GVector2D( 0 , 0 ); // Reset tile dimensions

	if ( this->currentTileSet.empty( ) || !this->currentMapObjects || this->currentMapObjects->empty( ) ) {
		return totalMapSize; // Not enough data to calculate
	}
	if ( this->currentTileSet.at( 0 ).empty( ) ) { // Check if the first row itself is empty
		return totalMapSize;
	}

	TileSet & tileSetRef = this->currentTileSet;
	auto & tileObjectsRef = *this->currentMapObjects;

	// Attempt to find a representative tile dimension from the first valid sprite.
	// This assumes a grid-based map with mostly uniform tile sizes.
	bool dimensionsFound = false;
	for ( size_t r = 0; r < tileSetRef.size( ) && !dimensionsFound; ++r ) {
		if ( tileSetRef[ r ].empty( ) ) continue;
		for ( size_t c = 0; c < tileSetRef[ r ].size( ) && !dimensionsFound; ++c ) {
			mapObjectType type = tileSetRef[ r ][ c ];
			if ( tileObjectsRef.count( type ) ) {
				auto mapObj = tileObjectsRef.at( type );
				if ( mapObj && mapObj->getSprite( ) && mapObj->getSprite( )->get( ) ) {
					GVector2D spriteSize = mapObj->getSprite( )->get( )->getSpriteSize( );
					if ( spriteSize.x > 0 && spriteSize.y > 0 ) {
						this->tileDimensions.x = spriteSize.x;
						this->tileDimensions.y = spriteSize.y;
						dimensionsFound = true;
					}
				}
			}
		}
	}

	if ( !dimensionsFound ) {
		// Fallback or error: Could not determine tile dimensions.
		// Map size calculation and room identification might not work correctly.
		// Consider logging an error here.
		return totalMapSize; // Return zero size as dimensions are unknown
	}

	// Calculate total map size based on determined tile dimensions and grid size
	int numRows = tileSetRef.size( );
	// Assuming all rows have the same number of columns as the first valid row
	int numCols = 0;
	for ( const auto & row : tileSetRef ) {
		if ( !row.empty( ) ) {
			numCols = row.size( );
			break;
		}
	}
	if ( numCols == 0 && !tileSetRef.empty( ) ) { // if all rows were empty, but tileSetRef itself wasn't
		numCols = tileSetRef[ 0 ].size( ); // default to first row's col count, which would be 0 if it's also empty
	}


	totalMapSize.x = static_cast< float >( numCols ) * this->tileDimensions.x;
	totalMapSize.y = static_cast< float >( numRows ) * this->tileDimensions.y;

	// Update door instance positions based on grid coordinates and tile dimensions
	this->doorInstances.clear( );
	for ( int r = 0; r < numRows; ++r ) {
		if ( tileSetRef[ r ].empty( ) ) continue;
		for ( int c = 0; c < tileSetRef[ r ].size( ); ++c ) {
			if ( tileSetRef[ r ][ c ] == mapObjectType::door ) {
				// Door position is its top-left corner in local map coordinates
				GVector2D doorPos( static_cast< float >( c ) * this->tileDimensions.x ,
					static_cast< float >( r ) * this->tileDimensions.y );
				this->doorInstances.emplace( doorPos , DoorInstanceData( ) );
			}
		}
	}
	return totalMapSize;
}

GVector2D gameMap::worldToTileCoordinates( GVector2D worldPos ) const {
	if ( this->tileDimensions.x == 0.0f || this->tileDimensions.y == 0.0f ) {
		return GVector2D( -1.0f , -1.0f ); // Invalid tile coordinates if dimensions are zero
	}
	// Adjust for map's world position to get coordinates relative to map origin
	GVector2D relativePos = worldPos - this->mapPosition;

	int tileX = static_cast< int >( floor( relativePos.x / this->tileDimensions.x ) );
	int tileY = static_cast< int >( floor( relativePos.y / this->tileDimensions.y ) );

	return GVector2D( static_cast< float >( tileX ) , static_cast< float >( tileY ) );
}

GVector2D gameMap::tileToWorldCoordinates( GVector2D tilePos ) const {
	// Calcula a posição relativa multiplicando as coordenadas do tile pelas dimensões
	GVector2D relativePos( tilePos.x * this->tileDimensions.x , tilePos.y * this->tileDimensions.y );

	// Adiciona a posição do mapa para obter as coordenadas do mundo
	GVector2D worldPos = relativePos + this->mapPosition;

	return worldPos;
}

// Helper function to determine if a tile is walkable for room identification purposes
bool gameMap::isTileWalkable( int tileX , int tileY ) {
	// --- Bounds Checking for the tile itself ---
	if ( tileY < 0 || static_cast< size_t >( tileY ) >= this->currentTileSet.size( ) ) {
		return false;
	}
	const auto & row = this->currentTileSet[ static_cast< size_t >( tileY ) ];
	if ( row.empty( ) || tileX < 0 || static_cast< size_t >( tileX ) >= row.size( ) ) {
		return false;
	}

	mapObjectType tileType = row[ static_cast< size_t >( tileX ) ];

	// For room identification, only 'ground' tiles can be part of a room.
	if ( tileType != mapObjectType::ground ) {
		return false;
	}

	// --- Corridor Check ---
	// Now, check if this 'ground' tile is actually a corridor piece.
	// Corridors are defined as 'ground' tiles horizontally flanked by specific wall types.
	// This prevents the room-finding algorithm (BFS) from "bleeding" through corridors.
	// We need to check the tiles to the left and right.
	if ( tileX > 0 && static_cast< size_t >( tileX ) < row.size( ) - 1 ) {
		mapObjectType leftTile = row[ static_cast< size_t >( tileX - 1 ) ];
		mapObjectType rightTile = row[ static_cast< size_t >( tileX + 1 ) ];

		// If it's flanked by the specific corridor walls, it's a corridor.
		if ( leftTile == mapObjectType::leftlateralTallWall &&
			rightTile == mapObjectType::rightlateralTallWall )
		{
			return false; // This is a corridor tile, so it's not part of a room.
		}
	}

	// If the tile is 'ground' and not a corridor, it's considered part of a room.
	return true;

	// Example: If open doors should connect rooms, you might add:
	// if (tileType == mapObjectType::door) {
	//     GVector2D doorWorldPos = this->mapPosition + 
	//                              GVector2D(tileX * this->tileDimensions.x, tileY * this->tileDimensions.y);
	//     if (isDoorUnlocked(doorWorldPos)) return true;
	// }

	return false;
}

void gameMap::identifyRooms( ) {
	if ( currentTileSet.empty( ) || tileDimensions.x <= 0 || tileDimensions.y <= 0 ) {
		roomGrid.clear( );
		nextRoomId = 0;
		return;
	}
	// Ensure first row is not empty if currentTileSet is not empty.
	// This check might be redundant if calculateTileSize ensures valid structure.
	if ( currentTileSet[ 0 ].empty( ) && currentTileSet.size( ) > 0 ) {
		roomGrid.clear( );
		nextRoomId = 0;
		return;
	}


	size_t numRows = currentTileSet.size( );
	size_t numCols = 0;
	// Find a non-empty row to determine numCols, robust against leading empty rows.
	for ( const auto & row : currentTileSet ) {
		if ( !row.empty( ) ) {
			numCols = row.size( );
			break;
		}
	}
	if ( numCols == 0 ) { // All rows are empty or tileSet is empty
		roomGrid.clear( );
		nextRoomId = 0;
		return;
	}


	// Initialize roomGrid: 0 means unvisited or not part of a room (e.g., a wall).
	roomGrid.assign( numRows , std::vector<int>( numCols , 0 ) );
	nextRoomId = 1; // Start actual room IDs from 1.

	for ( size_t r = 0; r < numRows; ++r ) {
		if ( currentTileSet[ r ].empty( ) ) continue; // Skip empty rows
		// Ensure column count matches for consistency, though BFS handles varying row lengths by bounds checks
		if ( currentTileSet[ r ].size( ) != numCols ) {
			// This case (jagged array) should ideally be handled or prevented earlier.
			// For now, we'll process up to numCols, or could resize roomGrid[r]
			// If resizing: roomGrid[r].assign(currentTileSet[r].size(), 0);
			// But identifyRooms assumes a rectangular grid from numCols found.
		}
		for ( size_t c = 0; c < std::min( numCols , currentTileSet[ r ].size( ) ); ++c ) { // Iterate up to the smaller of pre-determined numCols or actual row size
			if ( isTileWalkable( static_cast< int >( c ) , static_cast< int >( r ) ) && roomGrid[ r ][ c ] == 0 ) {
				// Start BFS for a new room
				std::queue<std::pair<int , int>> q;
				q.push( { static_cast< int >( c ), static_cast< int >( r ) } ); // Store as {tileX, tileY}
				roomGrid[ r ][ c ] = nextRoomId;

				while ( !q.empty( ) ) {
					std::pair<int , int> currentTile = q.front( );
					q.pop( );
					int currentX = currentTile.first;
					int currentY = currentTile.second;

					// Define neighbors (up, down, left, right)
					int dx[ ] = { 0, 0, 1, -1 }; // Change in X
					int dy[ ] = { 1, -1, 0, 0 }; // Change in Y

					for ( int i = 0; i < 4; ++i ) {
						int neighborX = currentX + dx[ i ];
						int neighborY = currentY + dy[ i ];

						// isTileWalkable performs bounds checking against currentTileSet
						if ( isTileWalkable( neighborX , neighborY ) ) {
							// Additional bounds check for roomGrid access needed if currentTileSet is jagged
							// and roomGrid was initialized uniformly based on max numCols.
							// isTileWalkable uses currentTileSet bounds. Here we need roomGrid bounds.
							if ( static_cast< size_t >( neighborY ) < roomGrid.size( ) &&
								static_cast< size_t >( neighborX ) < roomGrid[ neighborY ].size( ) &&
								roomGrid[ neighborY ][ neighborX ] == 0 )
							{
								roomGrid[ neighborY ][ neighborX ] = nextRoomId;
								q.push( { neighborX, neighborY } );
							}
						}
					}
				}
				nextRoomId++; // Increment for the next room found
			}
		}
	}
}

int gameMap::getRoomIdAtPosition( GVector2D worldPos ) {
	std::lock_guard<std::mutex> lock( this->mapMutex );

	if ( roomGrid.empty( ) ) return 0; // No rooms identified

	GVector2D tileCoords = worldToTileCoordinates( worldPos );
	int tileX = static_cast< int >( tileCoords.x );
	int tileY = static_cast< int >( tileCoords.y );

	// Check bounds against the roomGrid
	if ( tileY < 0 || static_cast< size_t >( tileY ) >= roomGrid.size( ) ||
		tileX < 0 || roomGrid[ static_cast< size_t >( tileY ) ].empty( ) || // Check if row in roomGrid is empty
		static_cast< size_t >( tileX ) >= roomGrid[ static_cast< size_t >( tileY ) ].size( ) ) {
		return 0; // Position is outside the map grid or on an unhandled tile
	}

	return roomGrid[ static_cast< size_t >( tileY ) ][ static_cast< size_t >( tileX ) ]; // Return room ID (0 if wall/unassigned)
}

bool gameMap::areInSameRoom( GVector2D worldPos1 , GVector2D worldPos2 ) {
	// getRoomIdAtPosition handles its own locking, so an explicit lock here
	// might be redundant if only reading. However, to ensure atomicity of reading 
	// both room IDs from a potentially changing roomGrid (if setMap can be called concurrently),
	// locking the entire operation is safer.

	int roomId1 = getRoomIdAtPosition( worldPos1 );
	int roomId2 = getRoomIdAtPosition( worldPos2 );

	// A room ID of 0 typically means the tile is not part of any room (e.g., it's a wall, or outside).
	// Thus, if either ID is 0, they cannot be in the same room.
	// Also, valid room IDs are positive (started from nextRoomId = 1).
	if ( roomId1 == 0 || roomId2 == 0 ) {
		return false;
	}

	return roomId1 == roomId2;
}

GVector2D gameMap::getMapSize( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->mapSize;
}

GVector2D gameMap::getTileDimensions( ) const {
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->tileDimensions;
}


bool gameMap::isDoorUnlocked( GVector2D position )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	// Note: Door positions are stored relative to map's local origin (0,0)
	if ( this->doorInstances.count( position ) ) { // Use count for std::map
		return this->doorInstances.at( position ).unlocked;
	}
	return false; // Default to false if door not found at exact position
}

void gameMap::setDoorUnlocked( GVector2D position , bool unlocked )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	if ( this->doorInstances.count( position ) ) {
		this->doorInstances.at( position ).unlocked = unlocked;
	}
}

DoorInstanceData * gameMap::getDoorInstanceData( GVector2D position )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	if ( this->doorInstances.count( position ) ) {
		return &this->doorInstances.at( position );
	}
	return nullptr;
}

std::map<GVector2D , DoorInstanceData> gameMap::getDoorInstancesCopy( )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->doorInstances;
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

	// First, a coarse check against the overall map bounding box
	GVector2D minMapWorldPos = this->mapPosition;
	GVector2D maxMapWorldPos = this->mapPosition + this->mapSize;

	if ( position.x > maxMapWorldPos.x || position.x < minMapWorldPos.x ||
		position.y > maxMapWorldPos.y || position.y < minMapWorldPos.y ) {
		return false;
	}

	// If tile dimensions are not set, we can't accurately check the tile type
	if ( this->tileDimensions.x <= 0 || this->tileDimensions.y <= 0 ) {
		// It's within map bounds but we can't check the specific tile type.
		// Depending on desired behavior, could return true or false.
		// Returning false as we can't confirm it's on 'ground'.
		return false;
	}
	if ( this->currentTileSet.empty( ) || !this->currentMapObjects || this->currentMapObjects->empty( ) ) {
		return false; // No map data to check against
	}

	// Convert world position to tile coordinates
	GVector2D tileCoords = worldToTileCoordinates( position );
	int c = static_cast< int >( tileCoords.x ); // Column index
	int r = static_cast< int >( tileCoords.y ); // Row index

	// Check if tile coordinates are within the bounds of the tileSet
	if ( r < 0 || static_cast< size_t >( r ) >= this->currentTileSet.size( ) ||
		c < 0 || this->currentTileSet[ static_cast< size_t >( r ) ].empty( ) || // check row empty
		static_cast< size_t >( c ) >= this->currentTileSet[ static_cast< size_t >( r ) ].size( ) ) {
		return false; // Tile coordinates are out of bounds
	}

	// Check if the tile at these coordinates is of type 'ground'
	mapObjectType tileTypeAtPos = this->currentTileSet[ static_cast< size_t >( r ) ][ static_cast< size_t >( c ) ];
	if ( this->currentMapObjects->count( tileTypeAtPos ) && ( tileTypeAtPos == mapObjectType::ground ) ) {
		return true;
	}

	if ( tileTypeAtPos == mapObjectType::door ) {
		// If it's a door, we can check if it's unlocked
		GVector2D doorWorldPos = tileToWorldCoordinates( GVector2D( static_cast< float >( c ) , static_cast< float >( r ) ) );
		if ( this->doorInstances.count( doorWorldPos ) && this->doorInstances.at( doorWorldPos ).unlocked ) {
			return true;
		}
	}


	return false; // Not ground or tile type not found in mapObjects
}

/**
 * @brief Verifica se uma determinada posição no mundo corresponde a um tile de corredor.
 *
 * Um corredor é identificado como um tile 'ground' que está diretamente entre
 * um 'leftlateralTallWall' e um 'rightlateralTallWall' na mesma linha.
 * Esta definição é baseada na lógica de geração de mapa em `generateDungeonTileSet`.
 *
 * @param worldPos A posição (coordenadas do mundo) a ser verificada.
 * @return true se a posição estiver em um tile de corredor, false caso contrário.
 */
bool gameMap::isCorridor( GVector2D worldPos )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );

	// Se as dimensões dos tiles não forem válidas ou o tileset estiver vazio, não pode ser um corredor.
	if ( this->tileDimensions.x <= 0 || this->tileDimensions.y <= 0 || this->currentTileSet.empty( ) ) {
		return false;
	}

	// Converte a posição do mundo para coordenadas de tile
	GVector2D tileCoords = worldToTileCoordinates( worldPos );
	int tileX = static_cast< int >( tileCoords.x );
	int tileY = static_cast< int >( tileCoords.y );

	// --- Validação de Limites ---
	// Verifica se a coordenada Y (linha) está dentro dos limites do grid.
	if ( tileY < 0 || static_cast< size_t >( tileY ) >= this->currentTileSet.size( ) ) {
		return false;
	}

	// Obtém a linha para verificar os limites da coluna. Pula se a linha estiver vazia.
	const auto & row = this->currentTileSet[ static_cast< size_t >( tileY ) ];
	if ( row.empty( ) ) {
		return false;
	}

	// Verifica se a coordenada X e seus vizinhos imediatos (esquerda e direita) estão dentro dos limites da linha.
	// Precisamos de tileX-1, tileX, e tileX+1, então tileX deve ser > 0 e < (tamanho-1).
	if ( tileX <= 0 || static_cast< size_t >( tileX ) >= row.size( ) - 1 ) {
		return false;
	}

	// --- Lógica Principal ---
	// Obtém o tipo do tile central e seus vizinhos laterais
	mapObjectType centerTile = row[ static_cast< size_t >( tileX ) ];
	mapObjectType leftTile = row[ static_cast< size_t >( tileX - 1 ) ];
	mapObjectType rightTile = row[ static_cast< size_t >( tileX + 1 ) ];

	// Um tile é considerado parte de um corredor se for 'ground' e estiver
	// ladeado por paredes laterais específicas.
	if ( centerTile == mapObjectType::ground &&
		leftTile == mapObjectType::leftlateralTallWall &&
		rightTile == mapObjectType::rightlateralTallWall )
	{
		return true;
	}

	return false;
}

GVector2D gameMap::getRandomPlace( int roomID ) {
	std::lock_guard<std::mutex> lock( this->mapMutex );

	// Verifica se a grade de salas está vazia ou se o ID da sala é inválido (IDs de sala começam em 1)
	if ( this->roomGrid.empty( ) || roomID <= 0 ) {
		// Log::Print("getRandomPlace: roomGrid está vazia ou roomID inválido."); // Opcional: log de erro
		return GVector2D( -1.0f , -1.0f ); // Retorna uma posição inválida para indicar erro
	}

	std::vector<GVector2D> candidateTiles;
	// Itera sobre a grade de salas para encontrar todos os ladrilhos que correspondem ao roomID
	for ( size_t r = 0; r < this->roomGrid.size( ); ++r ) {
		// Pula linhas vazias na grade de salas
		if ( this->roomGrid[ r ].empty( ) ) continue;

		for ( size_t c = 0; c < this->roomGrid[ r ].size( ); ++c ) {
			if ( this->roomGrid[ r ][ c ] == roomID ) {
				// Armazena as coordenadas do ladrilho (como float para GVector2D)
				candidateTiles.push_back( GVector2D( static_cast< float >( c ) , static_cast< float >( r ) ) );
			}
		}
	}

	// Se nenhum ladrilho for encontrado para o roomID especificado
	if ( candidateTiles.empty( ) ) {
		// Log::Print("getRandomPlace: Nenhum ladrilho encontrado para roomID %d", roomID); // Opcional: log de erro
		return GVector2D( -1.0f , -1.0f ); // Retorna uma posição inválida
	}

	// Configura o gerador de números aleatórios para selecionar um ladrilho
	std::random_device rd;
	std::mt19937 gen( rd( ) );
	std::uniform_int_distribution<> distrib( 0 , candidateTiles.size( ) - 1 );

	// Seleciona um índice aleatório da lista de ladrilhos candidatos
	int randomIndex = distrib( gen );
	GVector2D randomTileCoords = candidateTiles[ randomIndex ];

	// Converte as coordenadas do ladrilho selecionado para coordenadas do mundo
	GVector2D worldPos = this->tileToWorldCoordinates( randomTileCoords );

	// Ajusta a posição para o centro do ladrilho para um local mais "centralizado"
	if ( this->tileDimensions.x > 0 && this->tileDimensions.y > 0 ) {
		worldPos.x += this->tileDimensions.x * 0.5f;
		worldPos.y += this->tileDimensions.y * 0.5f;
	}

	return worldPos;
}

int gameMap::getNumRooms( ) const {
	std::lock_guard<std::mutex> lock( this->mapMutex );
	// nextRoomId é inicializado como 1 e incrementado após cada sala ser encontrada.
	// Se nextRoomId é 1, significa que nenhuma sala foi totalmente processada ainda (ou não há salas).
	// Se nextRoomId é N, significa que salas com IDs de 1 a N-1 foram encontradas.
	if ( nextRoomId <= 1 ) {
		return 0; // Nenhuma sala identificada ou antes da primeira sala ser contada.
	}
	return nextRoomId - 1; // Retorna o número de salas identificadas.
}
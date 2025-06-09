#include "gameMap.h"
#include <queue> // For std::queue in BFS room identification
#include <random>
#include <vector>
#include <algorithm> // Para std::sort
#include <map>

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

	// Garante que o mapObjectsHandler � v�lido e que getTileSet retorna um ponteiro v�lido
	mapObjectsHandler & handler = mapObjectsHandler::Get( );
	DungeonLayout * layout = handler.getDungeonLayout( type );

	const TileSet tileSetPtr = layout->tiles;
	if ( !tileSetPtr.empty( ) ) {
		this->currentTileSet = tileSetPtr; // Cria uma c�pia local
	}
	else {
		this->currentTileSet.clear( );
		// Lida com erro: conjunto de tiles n�o encontrado para o tipo dado
		// Voc� pode querer registrar isso em log ou lan�ar uma exce��o
	}

	this->currentMapObjects = handler.getAllMapObjects( type );
	if ( !this->currentMapObjects ) {
		// Lida com erro: objetos do mapa n�o encontrados
		// Voc� pode querer registrar isso em log ou lan�ar uma exce��o
		// Por enquanto, limpa os dados relevantes para evitar falhas
		this->currentTileSet.clear( );
	}

	this->mapSize = this->calculateTileSize( ); // Isso tamb�m definir� this->tileDimensions

	// Prossegue com a identifica��o da sala se o mapa e os tiles forem v�lidos
	if ( !this->currentTileSet.empty( ) &&
		!this->currentTileSet[ 0 ].empty( ) &&
		this->tileDimensions.x > 0 &&
		this->tileDimensions.y > 0 ) {
		this->identifyRooms( );
	}
	else {
		this->roomGrid.clear( );
		this->nextRoomId = 0; // Reseta o contador de ID da sala
		if ( this->tileDimensions.x <= 0 || this->tileDimensions.y <= 0 ) {
			// Log de erro: dimens�es do tile n�o puderam ser determinadas
		}
	}

	GVector2D spawnPoint = layout->spawnPos;
	spawnPoint = spawnPoint * this->tileDimensions;

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	if ( player != nullptr ) {
		player->setEntityPosition( spawnPoint );
	}
}

// Calcula o tamanho total do mapa e as dimens�es individuais dos tiles.
// Tamb�m atualiza as posi��es das inst�ncias de portas com base na grade.
GVector2D gameMap::calculateTileSize( ) {
	GVector2D totalMapSize( 0 , 0 );
	this->tileDimensions = GVector2D( 0 , 0 ); // Reseta as dimens�es do tile

	if ( this->currentTileSet.empty( ) || !this->currentMapObjects || this->currentMapObjects->empty( ) ) {
		return totalMapSize; // N�o h� dados suficientes para calcular
	}
	if ( this->currentTileSet.at( 0 ).empty( ) ) { // Verifica se a primeira linha est� vazia
		return totalMapSize;
	}

	TileSet & tileSetRef = this->currentTileSet;
	auto & tileObjectsRef = *this->currentMapObjects;

	// Tenta encontrar uma dimens�o de tile representativa a partir do primeiro sprite v�lido.
	// Isso assume um mapa baseado em grade com tamanhos de tile majoritariamente uniformes.
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
		// Fallback ou erro: N�o foi poss�vel determinar as dimens�es do tile.
		// O c�lculo do tamanho do mapa e a identifica��o da sala podem n�o funcionar corretamente.
		// Considere registrar um erro aqui.
		return totalMapSize; // Retorna tamanho zero, pois as dimens�es s�o desconhecidas
	}

	// Calcula o tamanho total do mapa com base nas dimens�es do tile determinadas e no tamanho da grade
	int numRows = tileSetRef.size( );
	// Assumindo que todas as linhas t�m o mesmo n�mero de colunas que a primeira linha v�lida
	int numCols = 0;
	for ( const auto & row : tileSetRef ) {
		if ( !row.empty( ) ) {
			numCols = row.size( );
			break;
		}
	}
	if ( numCols == 0 && !tileSetRef.empty( ) ) { // se todas as linhas estavam vazias, mas tileSetRef n�o
		numCols = tileSetRef[ 0 ].size( ); // padr�o para a contagem de colunas da primeira linha, que seria 0 se tamb�m estivesse vazia
	}


	totalMapSize.x = static_cast< float >( numCols ) * this->tileDimensions.x;
	totalMapSize.y = static_cast< float >( numRows ) * this->tileDimensions.y;

	// Atualiza as posi��es das inst�ncias de portas com base nas coordenadas da grade e dimens�es do tile
	this->doorInstances.clear( );
	for ( int r = 0; r < numRows; ++r ) {
		if ( tileSetRef[ r ].empty( ) ) continue;
		for ( size_t c = 0; c < tileSetRef[ r ].size( ); ++c ) {
			if ( tileSetRef[ r ][ c ] == mapObjectType::door ) {
				// A posi��o da porta � seu canto superior esquerdo nas coordenadas locais do mapa
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
		return GVector2D( -1.0f , -1.0f ); // Coordenadas de tile inv�lidas se as dimens�es forem zero
	}
	// Ajusta para a posi��o do mapa no mundo para obter coordenadas relativas � origem do mapa
	GVector2D relativePos = worldPos - this->mapPosition;

	int tileX = static_cast< int >( floor( relativePos.x / this->tileDimensions.x ) );
	int tileY = static_cast< int >( floor( relativePos.y / this->tileDimensions.y ) );

	return GVector2D( static_cast< float >( tileX ) , static_cast< float >( tileY ) );
}

GVector2D gameMap::tileToWorldCoordinates( GVector2D tilePos ) const {
	// Calcula a posi��o relativa multiplicando as coordenadas do tile pelas dimens�es
	GVector2D relativePos( tilePos.x * this->tileDimensions.x , tilePos.y * this->tileDimensions.y );

	// Adiciona a posi��o do mapa para obter as coordenadas do mundo
	GVector2D worldPos = relativePos + this->mapPosition;

	return worldPos;
}

// Fun��o auxiliar para determinar se um tile � caminh�vel para fins de identifica��o de sala
bool gameMap::isTileWalkable( int tileX , int tileY ) {
	// --- Verifica��o de limites para o pr�prio tile ---
	if ( tileY < 0 || static_cast< size_t >( tileY ) >= this->currentTileSet.size( ) ) {
		return false;
	}
	const auto & row = this->currentTileSet[ static_cast< size_t >( tileY ) ];
	if ( row.empty( ) || tileX < 0 || static_cast< size_t >( tileX ) >= row.size( ) ) {
		return false;
	}

	mapObjectType tileType = row[ static_cast< size_t >( tileX ) ];

	// Para identifica��o de sala, apenas tiles 'ground' podem fazer parte de uma sala.
	if ( tileType != mapObjectType::ground ) {
		return false;
	}

	// --- Verifica��o de Corredor ---
	// Agora, verifica se este tile 'ground' � na verdade uma pe�a de corredor.
	// Corredores s�o definidos como tiles 'ground' flanqueados horizontalmente por tipos de parede espec�ficos.
	// Isso impede que o algoritmo de busca de sala (BFS) "vaze" atrav�s dos corredores.
	// Precisamos verificar os tiles � esquerda e � direita.
	if ( tileX > 0 && static_cast< size_t >( tileX ) < row.size( ) - 1 ) {
		mapObjectType leftTile = row[ static_cast< size_t >( tileX - 1 ) ];
		mapObjectType rightTile = row[ static_cast< size_t >( tileX + 1 ) ];

		// Se for flanqueado pelas paredes de corredor espec�ficas, � um corredor.
		if ( leftTile == mapObjectType::leftlateralTallWall &&
			rightTile == mapObjectType::rightlateralTallWall )
		{
			return false; // Este � um tile de corredor, ent�o n�o faz parte de uma sala.
		}
	}

	// Se o tile for 'ground' e n�o for um corredor, � considerado parte de uma sala.
	return true;
}

/**
 * @brief Identifica todas as �reas de sala no mapa, atribui IDs de baixo para cima.
 *
 * O processo � o seguinte:
 * 1.  Percorre a grade de tiles para encontrar todas as �reas de sala distintas usando BFS.
 * Cada sala recebe um ID tempor�rio.
 * 2.  Para cada sala, armazena suas coordenadas de tile e sua coordenada Y mais baixa (mais ao sul).
 * 3.  Ordena as salas com base em suas coordenadas Y mais baixas em ordem decrescente.
 * Isso coloca a sala mais "embaixo" no mapa primeiro.
 * 4.  Percorre as salas ordenadas e atribui os IDs finais (1, 2, 3, ...), atualizando a `roomGrid`.
 */
void gameMap::identifyRooms( ) {
	if ( currentTileSet.empty( ) || tileDimensions.x <= 0 || tileDimensions.y <= 0 ) {
		roomGrid.clear( );
		nextRoomId = 0;
		return;
	}

	size_t numRows = currentTileSet.size( );
	size_t numCols = 0;
	for ( const auto & row : currentTileSet ) {
		if ( !row.empty( ) ) {
			numCols = row.size( );
			break;
		}
	}
	if ( numCols == 0 ) {
		roomGrid.clear( );
		nextRoomId = 0;
		return;
	}

	// Inicializa a roomGrid. 0 = n�o visitado/parede.
	roomGrid.assign( numRows , std::vector<int>( numCols , 0 ) );
	int tempRoomId = 1;

	// Estruturas para armazenar informa��es sobre as salas encontradas
	std::map<int , std::vector<std::pair<int , int>>> roomTiles;
	std::map<int , int> roomBottomY;

	// 1. Encontrar todas as salas e atribuir IDs tempor�rios
	for ( size_t r = 0; r < numRows; ++r ) {
		if ( currentTileSet[ r ].empty( ) || currentTileSet[ r ].size( ) != numCols ) continue;

		for ( size_t c = 0; c < numCols; ++c ) {
			if ( isTileWalkable( static_cast< int >( c ) , static_cast< int >( r ) ) && roomGrid[ r ][ c ] == 0 ) {
				// Nova sala encontrada, iniciar BFS
				std::queue<std::pair<int , int>> q;
				q.push( { static_cast< int >( c ), static_cast< int >( r ) } );
				roomGrid[ r ][ c ] = tempRoomId;

				std::vector<std::pair<int , int>> currentRoomTiles;
				int currentRoomMaxY = -1;

				while ( !q.empty( ) ) {
					std::pair<int , int> currentTile = q.front( );
					q.pop( );
					int currentX = currentTile.first;
					int currentY = currentTile.second;

					// Adicionar tile � lista da sala atual e verificar a coordenada Y mais baixa
					currentRoomTiles.push_back( { currentX, currentY } );
					if ( currentY > currentRoomMaxY ) {
						currentRoomMaxY = currentY;
					}

					int dx[ ] = { 0, 0, 1, -1 };
					int dy[ ] = { 1, -1, 0, 0 };

					for ( int i = 0; i < 4; ++i ) {
						int neighborX = currentX + dx[ i ];
						int neighborY = currentY + dy[ i ];

						if ( isTileWalkable( neighborX , neighborY ) &&
							static_cast< size_t >( neighborY ) < roomGrid.size( ) &&
							static_cast< size_t >( neighborX ) < roomGrid[ neighborY ].size( ) &&
							roomGrid[ neighborY ][ neighborX ] == 0 ) {
							roomGrid[ neighborY ][ neighborX ] = tempRoomId;
							q.push( { neighborX, neighborY } );
						}
					}
				}

				// Armazenar informa��es da sala encontrada
				roomTiles[ tempRoomId ] = currentRoomTiles;
				roomBottomY[ tempRoomId ] = currentRoomMaxY;
				tempRoomId++;
			}
		}
	}

	// 2. Ordenar os IDs das salas com base na sua posi��o Y mais baixa (de baixo para cima)
	std::vector<int> sortedRoomIds;
	for ( auto const & [id , y] : roomBottomY ) {
		sortedRoomIds.push_back( id );
	}

	std::sort( sortedRoomIds.begin( ) , sortedRoomIds.end( ) , [ & ] ( int a , int b ) {
		return roomBottomY[ a ] > roomBottomY[ b ]; // Ordena por Y m�ximo (decrescente)
		} );

	// 3. Atribuir IDs finais e permanentes com base na ordem ordenada
	nextRoomId = 1;
	for ( int id : sortedRoomIds ) {
		for ( const auto & tile : roomTiles[ id ] ) {
			roomGrid[ tile.second ][ tile.first ] = nextRoomId;
		}
		nextRoomId++;
	}
}


int gameMap::getRoomIdAtPosition( GVector2D worldPos ) {
	std::lock_guard<std::mutex> lock( this->mapMutex );

	if ( roomGrid.empty( ) ) return 0; // Nenhuma sala identificada

	GVector2D tileCoords = worldToTileCoordinates( worldPos );
	int tileX = static_cast< int >( tileCoords.x );
	int tileY = static_cast< int >( tileCoords.y );

	// Verifica os limites em rela��o � roomGrid
	if ( tileY < 0 || static_cast< size_t >( tileY ) >= roomGrid.size( ) ||
		tileX < 0 || roomGrid[ static_cast< size_t >( tileY ) ].empty( ) || // Verifica se a linha na roomGrid est� vazia
		static_cast< size_t >( tileX ) >= roomGrid[ static_cast< size_t >( tileY ) ].size( ) ) {
		return 0; // A posi��o est� fora da grade do mapa ou em um tile n�o tratado
	}

	return roomGrid[ static_cast< size_t >( tileY ) ][ static_cast< size_t >( tileX ) ]; // Retorna o ID da sala (0 se for parede/n�o atribu�do)
}

bool gameMap::areInSameRoom( GVector2D worldPos1 , GVector2D worldPos2 ) {
	// getRoomIdAtPosition lida com seu pr�prio travamento, ent�o um bloqueio expl�cito aqui
	// pode ser redundante se for apenas leitura. No entanto, para garantir a atomicidade da leitura
	// de ambos os IDs de sala de uma roomGrid potencialmente em mudan�a (se setMap puder ser chamado concorrentemente),
	// travar a opera��o inteira � mais seguro.

	int roomId1 = getRoomIdAtPosition( worldPos1 );
	int roomId2 = getRoomIdAtPosition( worldPos2 );

	// Um ID de sala 0 tipicamente significa que o tile n�o faz parte de nenhuma sala (por exemplo, � uma parede ou est� fora).
	// Assim, se qualquer um dos IDs for 0, eles n�o podem estar na mesma sala.
	// Al�m disso, os IDs de sala v�lidos s�o positivos (come�aram em nextRoomId = 1).
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
	// Nota: As posi��es das portas s�o armazenadas em rela��o � origem local do mapa (0,0)
	if ( this->doorInstances.count( position ) ) { // Usa count para std::map
		return this->doorInstances.at( position ).unlocked;
	}
	return false; // Padr�o para falso se a porta n�o for encontrada na posi��o exata
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

	// Primeiro, uma verifica��o geral contra a caixa delimitadora do mapa
	GVector2D minMapWorldPos = this->mapPosition;
	GVector2D maxMapWorldPos = this->mapPosition + this->mapSize;

	if ( position.x > maxMapWorldPos.x || position.x < minMapWorldPos.x ||
		position.y > maxMapWorldPos.y || position.y < minMapWorldPos.y ) {
		return false;
	}

	// Se as dimens�es do tile n�o estiverem definidas, n�o podemos verificar o tipo do tile com precis�o
	if ( this->tileDimensions.x <= 0 || this->tileDimensions.y <= 0 ) {
		// Est� dentro dos limites do mapa, mas n�o podemos verificar o tipo de tile espec�fico.
		// Dependendo do comportamento desejado, pode retornar verdadeiro ou falso.
		// Retornando falso, pois n�o podemos confirmar que est� em 'ground'.
		return false;
	}
	if ( this->currentTileSet.empty( ) || !this->currentMapObjects || this->currentMapObjects->empty( ) ) {
		return false; // Nenhum dado de mapa para verificar
	}

	// Converte a posi��o do mundo para coordenadas de tile
	GVector2D tileCoords = worldToTileCoordinates( position );
	int c = static_cast< int >( tileCoords.x ); // �ndice da coluna
	int r = static_cast< int >( tileCoords.y ); // �ndice da linha

	// Verifica se as coordenadas do tile est�o dentro dos limites do tileSet
	if ( r < 0 || static_cast< size_t >( r ) >= this->currentTileSet.size( ) ||
		c < 0 || this->currentTileSet[ static_cast< size_t >( r ) ].empty( ) || // verifica se a linha est� vazia
		static_cast< size_t >( c ) >= this->currentTileSet[ static_cast< size_t >( r ) ].size( ) ) {
		return false; // Coordenadas do tile est�o fora dos limites
	}

	// Verifica se o tile nessas coordenadas � do tipo 'ground'
	mapObjectType tileTypeAtPos = this->currentTileSet[ static_cast< size_t >( r ) ][ static_cast< size_t >( c ) ];
	if ( this->currentMapObjects->count( tileTypeAtPos ) && ( tileTypeAtPos == mapObjectType::ground ) ) {
		return true;
	}

	if ( tileTypeAtPos == mapObjectType::door ) {
		// Se for uma porta, podemos verificar se est� destrancada
		GVector2D doorWorldPos = tileToWorldCoordinates( GVector2D( static_cast< float >( c ) , static_cast< float >( r ) ) );
		if ( this->doorInstances.count( doorWorldPos ) && this->doorInstances.at( doorWorldPos ).unlocked ) {
			return true;
		}
	}


	return false; // N�o � ground ou tipo de tile n�o encontrado em mapObjects
}

/**
 * @brief Verifica se uma determinada posi��o no mundo corresponde a um tile de corredor.
 *
 * Um corredor � identificado como um tile 'ground' que est� diretamente entre
 * um 'leftlateralTallWall' e um 'rightlateralTallWall' na mesma linha.
 * Esta defini��o � baseada na l�gica de gera��o de mapa em `generateDungeonTileSet`.
 *
 * @param worldPos A posi��o (coordenadas do mundo) a ser verificada.
 * @return true se a posi��o estiver em um tile de corredor, false caso contr�rio.
 */
bool gameMap::isCorridor( GVector2D worldPos )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );

	// Se as dimens�es dos tiles n�o forem v�lidas ou o tileset estiver vazio, n�o pode ser um corredor.
	if ( this->tileDimensions.x <= 0 || this->tileDimensions.y <= 0 || this->currentTileSet.empty( ) ) {
		return false;
	}

	// Converte a posi��o do mundo para coordenadas de tile
	GVector2D tileCoords = worldToTileCoordinates( worldPos );
	int tileX = static_cast< int >( tileCoords.x );
	int tileY = static_cast< int >( tileCoords.y );

	// --- Valida��o de Limites ---
	// Verifica se a coordenada Y (linha) est� dentro dos limites do grid.
	if ( tileY < 0 || static_cast< size_t >( tileY ) >= this->currentTileSet.size( ) ) {
		return false;
	}

	// Obt�m a linha para verificar os limites da coluna. Pula se a linha estiver vazia.
	const auto & row = this->currentTileSet[ static_cast< size_t >( tileY ) ];
	if ( row.empty( ) ) {
		return false;
	}

	// Verifica se a coordenada X e seus vizinhos imediatos (esquerda e direita) est�o dentro dos limites da linha.
	// Precisamos de tileX-1, tileX, e tileX+1, ent�o tileX deve ser > 0 e < (tamanho-1).
	if ( tileX <= 0 || static_cast< size_t >( tileX ) >= row.size( ) - 1 ) {
		return false;
	}

	// --- L�gica Principal ---
	// Obt�m o tipo do tile central e seus vizinhos laterais
	mapObjectType centerTile = row[ static_cast< size_t >( tileX ) ];
	mapObjectType leftTile = row[ static_cast< size_t >( tileX - 1 ) ];
	mapObjectType rightTile = row[ static_cast< size_t >( tileX + 1 ) ];

	// Um tile � considerado parte de um corredor se for 'ground' e estiver
	// ladeado por paredes laterais espec�ficas.
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

	// Verifica se a grade de salas est� vazia ou se o ID da sala � inv�lido (IDs de sala come�am em 1)
	if ( this->roomGrid.empty( ) || roomID <= 0 ) {
		// Log::Print("getRandomPlace: roomGrid est� vazia ou roomID inv�lido."); // Opcional: log de erro
		return GVector2D( -1.0f , -1.0f ); // Retorna uma posi��o inv�lida para indicar erro
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
		return GVector2D( -1.0f , -1.0f ); // Retorna uma posi��o inv�lida
	}

	// Configura o gerador de n�meros aleat�rios para selecionar um ladrilho
	std::random_device rd;
	std::mt19937 gen( rd( ) );
	std::uniform_int_distribution<> distrib( 0 , candidateTiles.size( ) - 1 );

	// Seleciona um �ndice aleat�rio da lista de ladrilhos candidatos
	int randomIndex = distrib( gen );
	GVector2D randomTileCoords = candidateTiles[ randomIndex ];

	// Converte as coordenadas do ladrilho selecionado para coordenadas do mundo
	GVector2D worldPos = this->tileToWorldCoordinates( randomTileCoords );

	// Ajusta a posi��o para o centro do ladrilho para um local mais "centralizado"
	if ( this->tileDimensions.x > 0 && this->tileDimensions.y > 0 ) {
		worldPos.x += this->tileDimensions.x * 0.5f;
		worldPos.y += this->tileDimensions.y * 0.5f;
	}

	return worldPos;
}

int gameMap::getNumRooms( ) const {
	std::lock_guard<std::mutex> lock( this->mapMutex );
	// nextRoomId � inicializado como 1 e incrementado ap�s cada sala ser encontrada.
	// Se nextRoomId � 1, significa que nenhuma sala foi totalmente processada ainda (ou n�o h� salas).
	// Se nextRoomId � N, significa que salas com IDs de 1 a N-1 foram encontradas.
	if ( nextRoomId <= 1 ) {
		return 0; // Nenhuma sala identificada ou antes da primeira sala ser contada.
	}
	return nextRoomId - 1; // Retorna o n�mero de salas identificadas.
}

﻿#include "mapInitializer.h"

#include <vector>
#include <random>
#include <algorithm> // Para std::max, std::min, std::swap
#include <stdexcept> // Para std::runtime_error (opcional, para validação de entrada)
#include <iostream>  // Para debug, se necessário

#include  "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"
#include  "../../Handlers/LevelHandler/LevelHandler.h"
#include "../../Managers/gameResourceManager/gameResourceManager.h"

#include "../../../Utils/Log/Log.h"

using namespace std;


// Atalhos para os tipos de objeto do mapa para facilitar a leitura do código
mapObjectType d = mapObjectType::door;
mapObjectType t = mapObjectType::tallWall;
mapObjectType g = mapObjectType::ground;
mapObjectType n = mapObjectType::mapObjectNone;
mapObjectType r_wall = mapObjectType::rightlateralTallWall; // Renomeado de 'r' para evitar conflito
mapObjectType l_wall = mapObjectType::leftlateralTallWall; // Renomeado de 'l' para evitar conflito

// Configuração do gerador de números aleatórios
std::random_device global_rd;
std::mt19937 global_rng( global_rd( ) );

bool initializeMapObjects( mapType map , std::string mapName )
{
	for ( int i = 1; i < ( int ) mapObjectType::mapObjectCount; i++ ) {
		mapObjectType objectType = ( mapObjectType ) i;
		std::string objectTypeName = rMapObject::geMapObjectTypeName( objectType );
		Log::Print( "Initializing map objects of type: %s for map: %s" , objectTypeName.c_str( ) , mapName.c_str( ) );

		std::shared_ptr<rSprite> mapAsset =
			_gameResourceManager.getMapManager( )->getMapAsset( mapName + "_" + objectTypeName );

		if ( mapAsset == nullptr || mapAsset.get( ) == nullptr ) {
			Log::Print( "Failed to load map asset for type: %s in map: %s" , objectTypeName.c_str( ) , mapName.c_str( ) );
			return false;
		}
		std::shared_ptr<rMapObject> mapObject = std::make_shared<rMapObject>( objectType , mapAsset );
		mapObjectsHandler::Get( ).addGameObject( map , objectType , mapObject );
	}

	return true;
}

// Função auxiliar para obter um inteiro aleatório no intervalo [min, max] (inclusivo)
int getRandomInt( int min_val , int max_val ) {
	if ( min_val > max_val ) {
		std::swap( min_val , max_val );
	}
	std::uniform_int_distribution<> distrib( min_val , max_val );
	return distrib( global_rng );
}

// Função auxiliar para obter uma dimensão ímpar aleatória no intervalo [min_dim, max_dim]
// Garante que a dimensão seja pelo menos 3.
int getRandomOddDimension( int min_dim , int max_dim ) {
	min_dim = std::max( 3 , min_dim );
	max_dim = std::max( min_dim , max_dim );

	int actual_min_odd = ( min_dim % 2 != 0 ) ? min_dim : min_dim + 1;
	if ( actual_min_odd > max_dim ) {
		if ( max_dim % 2 != 0 && max_dim >= 3 ) return max_dim;
		if ( min_dim % 2 != 0 ) return min_dim;
		return 3;
	}

	int actual_max_odd = ( max_dim % 2 != 0 ) ? max_dim : max_dim - 1;
	if ( actual_max_odd < actual_min_odd ) {
		if ( max_dim % 2 != 0 && max_dim >= 3 ) return max_dim;
		if ( min_dim % 2 != 0 ) return min_dim;
		return 3;
	}

	int num_odd_choices = ( actual_max_odd - actual_min_odd ) / 2 + 1;
	std::uniform_int_distribution<> distrib( 0 , num_odd_choices - 1 );
	return actual_min_odd + 2 * distrib( global_rng );
}


/**
 * Gera um TileSet representando um dungeon com salas e corredores, e uma posição de spawn.
 *
 * @param numRooms O número de salas a serem geradas.
 * @param minRoomW A largura mínima total de uma sala (incluindo paredes laterais, deve ser >=3 e ímpar).
 * @param maxRoomW A largura máxima total de uma sala.
 * @param minRoomH A altura mínima total de uma sala (incluindo paredes superior/inferior, deve ser >=3).
 * @param maxRoomH A altura máxima total de uma sala.
 * @param corridorLen O comprimento (altura) dos corredores entre as salas.
 * @return Um DungeonLayout contendo o TileSet e a Posição de Spawn.
 */
DungeonLayout generateDungeonTileSet(
	int numRooms ,
	int minRoomW , int maxRoomW ,
	int minRoomH , int maxRoomH ,
	int corridorLen ) {

	if ( numRooms <= 0 ) {
		// Retorna layout vazio com spawn inválido se não houver salas.
		return { {}, {-1, -1} };
	}
	// Validação básica (pode ser expandida)
	if ( minRoomW > maxRoomW || minRoomH > maxRoomH || corridorLen < 0 ) {
		// std::cerr << "Parâmetros de dimensão inválidos." << std::endl;
		return { {}, {-1,-1} }; // Retorna layout vazio com spawn inválido
	}

	std::vector<std::pair<int , int>> room_dimensions( numRooms );
	int overall_max_width = 0;
	int total_map_height = 0;

	// 1. Pré-calcular dimensões das salas e do mapa
	for ( int i = 0; i < numRooms; ++i ) {
		int rW = getRandomOddDimension( minRoomW , maxRoomW );
		int rH = getRandomInt( minRoomH , maxRoomH );
		rH = std::max( 3 , rH );

		room_dimensions[ i ] = { rW, rH };
		overall_max_width = std::max( overall_max_width , rW );
		total_map_height += rH;
		if ( i < numRooms - 1 ) {
			total_map_height += std::max( 0 , corridorLen );
		}
	}

	overall_max_width = std::max( 3 , overall_max_width );
	if ( overall_max_width % 2 == 0 ) {
		overall_max_width++;
	}

	int universal_door_map_x = ( overall_max_width - 1 ) / 2;

	// Calcular Posição de Spawn (centro da última sala)

	//GVector2D spawnPos = { -1, -1 }; // Padrão inválido
	//if ( numRooms > 0 ) {
	//	int firstRoomW = room_dimensions[ 0 ].first;
	//	int firstRoomH = room_dimensions[ 0 ].second;

	//	// A primeira sala começa na coluna `room_start_x` e linha 0 do mapa.
	//	int first_room_door_offset_in_wall = ( firstRoomW - 1 ) / 2;
	//	int first_room_start_x_on_map = universal_door_map_x - first_room_door_offset_in_wall;

	//	// Posição de spawn relativa ao mapa geral
	//	spawnPos.x = first_room_start_x_on_map + ( firstRoomW / 2 ); // Centro X da primeira sala
	//	spawnPos.y = firstRoomH / 2; // Centro Y da primeira sala (offset Y da primeira sala é 0)

	//	// Verificação de limites (embora o spawn deva estar dentro da sala, que estará no mapa)
	//	spawnPos.x = std::max( 0 , std::min( (int)spawnPos.x , overall_max_width - 1 ) );
	//	spawnPos.y = std::max( 0 , std::min( ( int ) spawnPos.y , room_dimensions[ 0 ].second - 1 ) ); // Y relativo ao início do mapa
	//}

	//// 2. Inicializar o TileSet final com 'mapObjectNone'
	//TileSet finalTileSet( total_map_height , std::vector<mapObjectType>( overall_max_width , n ) );




	// Calcular Posição de Spawn (centro da última sala)
	GVector2D spawnPos = { -1, -1 }; // Padrão inválido
	if ( numRooms > 0 ) {
		int idxLastRoom = numRooms - 1;
		int lastRoomW = room_dimensions[ idxLastRoom ].first;
		int lastRoomH = room_dimensions[ idxLastRoom ].second;

		// Calcular o X inicial da última sala no mapa
		int last_room_door_offset_in_wall = ( lastRoomW - 1 ) / 2;
		int last_room_start_x_on_map = universal_door_map_x - last_room_door_offset_in_wall;

		// Calcular o Y inicial da última sala no mapa
		// Este offset representa a soma das alturas das salas anteriores e dos corredores entre elas.
		int y_start_on_map_of_last_room = 0;
		for ( int k = 0; k < idxLastRoom; ++k ) { // Itera sobre todas as salas ANTES da última
			y_start_on_map_of_last_room += room_dimensions[ k ].second; // Adiciona altura da sala k
			if ( corridorLen > 0 ) { // Adiciona comprimento do corredor DEPOIS da sala k (que é ANTES da sala k+1)
				y_start_on_map_of_last_room += corridorLen;
			}
		}

		// Posição de spawn relativa ao mapa geral (centro da última sala)
		spawnPos.x = last_room_start_x_on_map + ( lastRoomW / 2 );
		spawnPos.y = y_start_on_map_of_last_room + ( lastRoomH / 2 );

		// Verificação de limites para a posição de spawn
		// Garante que o spawn X esteja dentro dos limites do mapa
		spawnPos.x = std::max( 0 , std::min( (int)spawnPos.x , overall_max_width - 1 ) );

		// Garante que o spawn Y esteja dentro dos limites verticais da última sala.
		// A última sala começa em y_start_on_map_of_last_room e tem altura lastRoomH.
		// O spawn Y deve estar entre y_start_on_map_of_last_room e y_start_on_map_of_last_room + lastRoomH - 1.
		spawnPos.y = std::max( y_start_on_map_of_last_room , std::min( (int)spawnPos.y , y_start_on_map_of_last_room + lastRoomH - 1 ) );
	}

	// 2. Inicializar o TileSet final com 'mapObjectNone'
	TileSet finalTileSet( total_map_height , std::vector<mapObjectType>( overall_max_width , n ) );
	

	int current_y_offset = 0;

	// 3. Gerar cada sala e corredor
	for ( int i = 0; i < numRooms; ++i ) {
		int roomW = room_dimensions[ i ].first;
		int roomH = room_dimensions[ i ].second;

		int door_offset_in_room_wall = ( roomW - 1 ) / 2;
		int room_start_x = universal_door_map_x - door_offset_in_room_wall;

		// 3a. Desenhar corredor 
		if ( i > 0 && corridorLen > 0 ) {
			for ( int cy = 0; cy < corridorLen; ++cy ) {
				if ( current_y_offset + cy < total_map_height && universal_door_map_x > 0 && universal_door_map_x < overall_max_width - 1 ) {
					finalTileSet[ current_y_offset + cy ][ universal_door_map_x - 1 ] = l_wall;
					finalTileSet[ current_y_offset + cy ][ universal_door_map_x ] = g;
					finalTileSet[ current_y_offset + cy ][ universal_door_map_x + 1 ] = r_wall;
				}
			}
			current_y_offset += corridorLen;
		}

		// 3b. Desenhar a sala atual
		// Parede Superior
		if ( current_y_offset < total_map_height ) {
			for ( int x = 0; x < roomW; ++x ) {
				int map_x = room_start_x + x;
				if ( map_x >= 0 && map_x < overall_max_width ) {
					if ( x == 0 ) finalTileSet[ current_y_offset ][ map_x ] = l_wall;
					else if ( x == roomW - 1 ) finalTileSet[ current_y_offset ][ map_x ] = r_wall;
					else {
						if ( i > 0 && map_x == universal_door_map_x ) {
							finalTileSet[ current_y_offset ][ map_x ] = d;
						}
						else {
							finalTileSet[ current_y_offset ][ map_x ] = t;
						}
					}
				}
			}
		}

		// Partes do Meio da Sala
		for ( int ry = 1; ry < roomH - 1; ++ry ) {
			int current_row_y = current_y_offset + ry;
			if ( current_row_y < total_map_height ) {
				for ( int x = 0; x < roomW; ++x ) {
					int map_x = room_start_x + x;
					if ( map_x >= 0 && map_x < overall_max_width ) {
						if ( x == 0 ) finalTileSet[ current_row_y ][ map_x ] = l_wall;
						else if ( x == roomW - 1 ) finalTileSet[ current_row_y ][ map_x ] = r_wall;
						else finalTileSet[ current_row_y ][ map_x ] = g;
					}
				}
			}
		}

		// Parede Inferior
		int bottom_wall_y = current_y_offset + roomH - 1;
		if ( bottom_wall_y < total_map_height ) {
			for ( int x = 0; x < roomW; ++x ) {
				int map_x = room_start_x + x;
				if ( map_x >= 0 && map_x < overall_max_width ) {
					if ( x == 0 ) finalTileSet[ bottom_wall_y ][ map_x ] = l_wall;
					else if ( x == roomW - 1 ) finalTileSet[ bottom_wall_y ][ map_x ] = r_wall;
					else {
						if ( i < numRooms - 1 && map_x == universal_door_map_x ) {
							finalTileSet[ bottom_wall_y ][ map_x ] = d;
						}
						else {
							finalTileSet[ bottom_wall_y ][ map_x ] = t;
						}
					}
				}
			}
		}
		current_y_offset += roomH;
	}
	return { finalTileSet, spawnPos, numRooms };
}

// Função para imprimir o TileSet (para teste)
void printTileSet( const TileSet & tileSet , const GVector2D & spawnPos ) {
	std::cout << "TileSet (" << tileSet.size( ) << " linhas x "
		<< ( tileSet.empty( ) ? 0 : tileSet[ 0 ].size( ) ) << " colunas):" << std::endl;

	for ( size_t r = 0; r < tileSet.size( ); ++r ) {
		for ( size_t c = 0; c < tileSet[ r ].size( ); ++c ) {
			char out_char = '?';
			mapObjectType cell = tileSet[ r ][ c ];
			if ( static_cast< int >( r ) == spawnPos.x && static_cast< int >( c ) == spawnPos.y && cell == mapObjectType::ground ) {
				out_char = 'S'; // 'S' para Posição de Spawn se for chão
			}
			else {
				switch ( cell ) {
				case mapObjectType::door: out_char = 'D'; break;
				case mapObjectType::tallWall: out_char = 'T'; break;
				case mapObjectType::ground: out_char = '.'; break;
				case mapObjectType::mapObjectNone: out_char = ' '; break;
				case mapObjectType::rightlateralTallWall: out_char = ']'; break;
				case mapObjectType::leftlateralTallWall: out_char = '['; break;
				}
			}
			std::cout << out_char;
		}
		std::cout << std::endl;
	}
	std::cout << "Posição de Spawn (X, Y): (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;

}

TileSet corrigirTilesetPorCelulas(
	const TileSet & original ,
	unordered_map<mapObjectType , shared_ptr<rMapObject>> * tileObjects
) {

	//Descobre quantas colunas (em termos de entradas originais) existem
	size_t maxCols = 0;
	for ( auto const & row : original ) {
		if ( row.size( ) > maxCols )
			maxCols = row.size( );
	}

	// Normaliza cada linha para ter exatamente maxCols entradas,
	// preenchendo com mapObjectNone se faltar.
	TileSet normalized = original;
	for ( auto & row : normalized ) {
		if ( row.size( ) < maxCols ) {
			row.resize( maxCols , mapObjectType::mapObjectNone );
		}
	}

	return normalized;
}

bool mapInitializer::initialize( ) {
	// Load the map objects from the specified map file
	std::vector<std::pair<mapType , std::string>> maps = {
		{ mapType::firstMap, "firstMap" },
		{ mapType::secondMap, "secondMap" },
		{ mapType::thirdMap, "thirdMap" },
		{ mapType::bossMap, "bossMap" },
	};

	for ( auto & map : maps ) {
		if ( !initializeMapObjects( map.first , map.second ) ) {
			Log::Print( "Failed to initialize map objects for %s" , map.second.c_str( ) );
			return false;
		}

		int numRooms = 7;
		int minRoomW = 10 , maxRoomW = 15; // Larguras devem ser ímpares e >= 3
		int minRoomH = 10 , maxRoomH = 15; // Alturas devem ser >= 3
		int corridorLen = 10;

		DungeonLayout layout = generateDungeonTileSet( numRooms , minRoomW , maxRoomW , minRoomH , maxRoomH , corridorLen );
		//printTileSet( layout.tiles , layout.spawnPos );

		mapObjectsHandler::Get( ).addDungeonLayout( map.first , layout );
	}

	return true; // All map objects initialized successfully
}
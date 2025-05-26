#include "collisionManager.h"

CollisionManager & CollisionManager::Get( ) {
	static CollisionManager instance;
	return instance;
}

void CollisionManager::UpdateEntities( std::vector<CBaseEntity *> & entities ) {
	std::lock_guard<std::mutex> lock( mutex );

	spatialGrid.clear( );
	allEntities = entities;

	for ( auto * entity : entities ) {
		if ( !entity ) continue;
		GridCell cell = {
			static_cast< int >( std::floor( entity->getEntityPosition( ).x / GRID_CELL_SIZE ) ),
			static_cast< int >( std::floor( entity->getEntityPosition( ).y / GRID_CELL_SIZE ) )
		};
		spatialGrid[ cell ].insert( entity );
	}
}

std::vector<CBaseEntity *> CollisionManager::GetNearbyEntities( const GVector2D & pos ) {
	std::vector<CBaseEntity *> result;
	std::lock_guard<std::mutex> lock( mutex );

	GridCell cell = {
		static_cast< int >( std::floor( pos.x / GRID_CELL_SIZE ) ),
		static_cast< int >( std::floor( pos.y / GRID_CELL_SIZE ) )
	};

	for ( int dx = -1; dx <= 1; ++dx ) {
		for ( int dy = -1; dy <= 1; ++dy ) {
			GridCell neighbor = { cell.x + dx, cell.y + dy };
			auto it = spatialGrid.find( neighbor );
			if ( it != spatialGrid.end( ) ) {
				result.insert( result.end( ) , it->second.begin( ) , it->second.end( ) );
			}
		}
	}

	return result;
}

bool CollisionManager::checkCollision( CBaseEntity * a , CBaseEntity * b , GVector2D newPosA ) {
	GVector2D sizeA = a->getEntityAnimations( )->getCurrentTextureSize( );
	GVector2D posB = b->getEntityPosition( );
	GVector2D sizeB = b->getEntityAnimations( )->getCurrentTextureSize( );

	return ( newPosA.x < posB.x + sizeB.x &&
		newPosA.x + sizeA.x > posB.x &&
		newPosA.y < posB.y + sizeB.y &&
		newPosA.y + sizeA.y > posB.y );
}

bool CollisionManager::checkSpacialCollision( CBaseEntity * a , GVector2D Position , GVector2D Size ) {
	GVector2D posB = a->getEntityPosition( );
	GVector2D sizeB = a->getEntityAnimations( )->getCurrentTextureSize( );

	return ( Position.x < posB.x + sizeB.x &&
		Position.x + Size.x > posB.x &&
		Position.y < posB.y + sizeB.y &&
		Position.y + Size.y > posB.y );
}


bool CollisionManager::CanMoveTo( CBaseEntity * entity , GVector2D & newPos ) {
	auto nearby = GetNearbyEntities( newPos );
	for ( auto * other : nearby ) {
		if ( other != entity && other->isAlive( ) ) {
			if ( checkCollision( entity , other , newPos ) ) {
				return false;
			}
		}
	}
	return true;
}

bool CollisionManager::isSpaceFree( GVector2D Position , GVector2D Size ) {
	auto nearby = GetNearbyEntities( Position );
	for ( auto * other : nearby ) {
		if ( checkSpacialCollision( other , Position , Size ) ) {
			return false;
		}
	}
	return true;
}

std::vector<CBaseEntity *> CollisionManager::GetAllEntities( ) {
	std::lock_guard<std::mutex> lock( mutex );
	return this->allEntities;
}

void CollisionManager::ProcessCollisions( ) {
	std::lock_guard<std::mutex> lock( mutex );

	for ( size_t i = 0; i < allEntities.size( ); ++i ) {
	
		
		for ( size_t j = i + 1; j < allEntities.size( ); ++j ) {

			CBaseEntity * b = allEntities[ j ];
			CBaseEntity * a = allEntities[ i ];

			if ( !a || !b || !a->isAlive( ) || !b->isAlive( ) ) 
				continue;
			if ( checkCollision( a , b , a->getEntityPosition( ) ) ) {
				//a->setEntityPosition( GVector2D(0 , 0) );
			}
		}
	}
}

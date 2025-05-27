#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>
#include <set>
#include <cmath>
#include "../../SDK/Entities/CBaseEntity/CBaseEntity.h"

class CollisionManager {
public:
    static CollisionManager & Get( );

    void UpdateEntities( std::vector<CBaseEntity *> entities );
    bool CanMoveTo( CBaseEntity * entity , GVector2D & newPos );
    void ProcessCollisions( );


    std::vector<CBaseEntity *> GetNearbyEntities( const GVector2D & pos );
    std::vector<CBaseEntity *> GetAllEntities( );
    bool checkCollision( CBaseEntity * a , CBaseEntity * b , GVector2D newPosA );
    bool checkSpacialCollision( CBaseEntity * a , GVector2D pos , GVector2D size );
    bool isSpaceFree( GVector2D pos , GVector2D size );

private:
    CollisionManager( ) = default;



    struct GridCell {
        int x , y;
        bool operator==( const GridCell & other ) const { return x == other.x && y == other.y; }
    };

    struct GridCellHash {
        std::size_t operator()( const GridCell & c ) const {
            return std::hash<int>( )( c.x ) ^ ( std::hash<int>( )( c.y ) << 1 );
        }
    };

    using EntitySet = std::set<CBaseEntity *>;

    std::unordered_map<GridCell , EntitySet , GridCellHash> spatialGrid;
    std::vector<CBaseEntity *> allEntities;
    std::mutex mutex;

    static constexpr float GRID_CELL_SIZE = 128.f;
};

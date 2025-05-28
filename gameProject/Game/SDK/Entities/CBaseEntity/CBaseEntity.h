#pragma once
#include <string>
#include "../../math/Vector2D/GVector2D.h"
#include "../../Animation/CBaseEntityAnimation/CBaseEntityAnimation.h"
#include "../../math/gAngle/GAngle.h"

#include <mutex>

enum CBaseEntityType {
	ITEM,
	PLAYER,
	MOB,
	ATTACK
};

enum CBaseEntityState {
	STOPPED = 1 << 0 , // 00001
	MOVING = 1 << 1 , // 00010
	RUNNING = 1 << 2 , // 00100
	ATTACKING = 1 << 3 , // 00100
	HURT = 1 << 4 , // 01000
	DEAD = 1 << 5  // 10000
};

enum CBaseEntityMovementDirection {
	MOVEMENT_FORWARD,
	MOVEMENT_BACKWARD,
	MOVEMENT_LEFT,
	MOVEMENT_RIGHT
};

class CBaseEntityMovement {
public:
	int dx;
	int dy;
};

struct CBaseEntityConstructor {
	std::string Name;

	GVector2D entityPosition;
	int health;
	int movementSpeed;

	CBaseEntityType entityType;
	CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;
	CBaseEntityAnimationConstructor entityAnimations;
};


class CBaseEntityHitbox {
	GVector2D hitboxSize;
public:
	CBaseEntityHitbox( ) = default;
	CBaseEntityHitbox( GVector2D size ) {
		this->hitboxSize = size;
	}

	GVector2D getHitboxSize( ) {
		return this->hitboxSize;
	}
};
enum DIRECTION {
	LEFT ,
	RIGHT ,
	FORWARD ,
	BACKWARD
};

class CBaseEntity {

    std::mutex cBaseMutex;
    std::string Name;
    GVector2D entityPosition;
    int health;
    int movementSpeed;
    CBaseEntityType entityType;
    CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;
    DIRECTION entityLookingDirection;
    std::uint32_t entityState = STOPPED;
    CBaseEntityAnimation entityAnimations;
    std::vector<CBaseEntityMovementDirection> movementsRequest;
    GAngle lookingAngle;
    float movementAngle;
    CBaseEntityHitbox entityHitbox;
    float maxHealth = 100;
    bool beingHit = false;
    bool finishedDeathAnimation = false;
    bool sprinting = false;
    std::chrono::steady_clock::time_point lastMoveTime;

public:

    // Construtores e operadores
    CBaseEntity( const CBaseEntity & other );
    CBaseEntity( CBaseEntityConstructor builder );

    // Atualização e animação
    virtual void updateEntity( );
    static CBaseEntityAnimationType getAnimationTypeBasedOnStateAndDirection( std::uint32_t states , DIRECTION entityDirection );

    // Direção e movimento
    float getEntityLookingDirectionBaseAngle( );
    bool hasMovementRequest( );
    void addMoveRequest( CBaseEntityMovementDirection movement );
    void clearMovementRequest( );
    void move( );

    // Estado de vida e animação de morte
    bool isAlive( );
    bool deathAnimationFinished( );
    void setDeathAnimationFinished( bool finished );

    // Ângulos e velocidade
    float getMovementAngle( );
    GAngle getLookingAngle( );
    float getMovementSpeed( ) { return movementSpeed; }


    // Getters
    std::string GetEntityName( );
    GVector2D getEntityPosition( );
    CBaseEntityType getEntityType( );
    CBaseEntityMovementDirection getEntityMovementDirection( );
    CBaseEntityAnimation * getEntityAnimations( );
    int getHealth( );
    CBaseEntityHitbox getHitbox( );
    float getMaxHealth( );
    DIRECTION getEntityLookingDirection( );

    // Setters
    void setEntityLookingDirection( DIRECTION direction );
    void setLookingAngle( float degrees );
    void setEntityMovementDirection( CBaseEntityMovementDirection move );
    void setHealth( int health );
    void setEntityPosition( GVector2D pos );

    // Sprint
    bool isSprinting( );
    void setSprinting( bool sprinting );

    // Estados da entidade
    void addEntityState( CBaseEntityState state );
    void removeEntityState( CBaseEntityState state );
    bool hasEntityState( CBaseEntityState state ) const;
    void clearEntityStates( );
    std::uint32_t getEntityStates( );
    void setEntityStates( std::uint32_t states );

    // Hit e dano
    void Hit( int damage );
    bool isBeingHit( );
    void stopBeingHit( );

    // Utilitários
    std::string getEntityStateAsString( ) const;
};
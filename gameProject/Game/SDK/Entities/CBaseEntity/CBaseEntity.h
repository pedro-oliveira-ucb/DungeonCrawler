#pragma once
#include <string>
#include "../../math/Vector2D/GVector2D.h"
#include "../../Animation/CBaseEntityAnimation/CBaseEntityAnimation.h"
#include "../../math/gAngle/GAngle.h"

#include <mutex>

enum CBaseEntityType {
	ITEM,
    TRAP,
	PLAYER,
	MOB,
	ATTACK
};

enum CBaseEntityState {
	STOPPED = 1 << 0 , // 00001
	MOVING = 1 << 1 , // 00010
	RUNNING = 1 << 2 , // 00100
	ATTACKING = 1 << 3 , // 00100
	MAGIC_ATTACKING = 1 << 4 , // 001000
	HURT = 1 << 5 , // 010000
	DEAD = 1 << 6  // 100000
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


class CBaseEntityHitbox {
    GVector2D hitboxSize;
public:
	CBaseEntityHitbox( ) : hitboxSize( 0 , 0 ) { }
    CBaseEntityHitbox( GVector2D size ) {
        this->hitboxSize = size;
    }

    GVector2D getHitboxSize( ) {
        return this->hitboxSize;
    }
};

struct CBaseEntityConstructor {
	std::string Name;

	GVector2D entityPosition;
	int health;
	int movementSpeed;

	CBaseEntityType entityType;
	CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;
	CBaseEntityAnimationConstructor entityAnimations;
    CBaseEntityHitbox entityHitbox;
};


enum DIRECTION {
    FORWARD ,
    BACKWARD ,
	LEFT ,
	RIGHT ,
    DIRECTION_COUNT
};

class CBaseEntity {

    mutable std::mutex cBaseMutex;
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
    int maxHealth = 100;
    bool beingHit = false;
    bool finishedDeathAnimation = false;
    bool sprinting = false;

    float currentStamina = 100.f;
	float maxStamina = 100.f;
	float staminaRegenRate = 20.0f;
    float staminaLossRate = 20.0f;
    
    float defense = 0.0f;

    double lastMoveTime;
public:

    // Construtores e operadores
    CBaseEntity( const CBaseEntity & other );
    CBaseEntity( CBaseEntityConstructor builder );

    // Atualiza��o e anima��o
    virtual void updateEntity( );
    static CBaseEntityAnimationType getAnimationTypeBasedOnStateAndDirection( std::uint32_t states , DIRECTION entityDirection );

    // Dire��o e movimento
    float getEntityLookingDirectionBaseAngle( ) const;
    bool hasMovementRequest( ) const;
    void addMoveRequest( CBaseEntityMovementDirection movement );
    void clearMovementRequest( );
    void move( );

    // Estado de vida e anima��o de morte
    bool isAlive( ) const;
    bool deathAnimationFinished( ) const;
    void setDeathAnimationFinished( bool finished );

    // �ngulos e velocidade
    float getMovementAngle( ) const;
    GAngle getLookingAngle( ) const;
    int getMovementSpeed( ) const;

    // Getters
    std::string GetEntityName( ) const;
    GVector2D getEntityPosition( ) const;
    CBaseEntityType getEntityType( ) const;
    CBaseEntityMovementDirection getEntityMovementDirection( ) const;
    CBaseEntityAnimation * getEntityAnimations( );
    int getHealth( ) const;
    CBaseEntityHitbox getHitbox( ) const;
    int getMaxHealth( ) const;
    DIRECTION getEntityLookingDirection( ) const;
    float getCurrentStamina( ) const;
	float getMaxStamina( ) const;
	float getStaminaRegenRate( ) const;
	float getStaminaLossRate( ) const;
	float getEntityDefense( ) const;


    // Setters
    void setEntityLookingDirection( DIRECTION direction );
    void setLookingAngle( float degrees );
    void setEntityMovementDirection( CBaseEntityMovementDirection move );
    void setHealth( int health );
    void setEntityPosition( GVector2D pos );
    void setCurrentStamina( float stamina );
	void setEntityMaxStamina( float stamina );
	void setEntityMaxHealth( int health );
	void setStaminaRegenRate( float rate );
	void setStaminaLossRate( float rate );
    void setMovementSpeed( float speed );
    void setEntityDefense( float defense );


    // Sprint
    bool isSprinting( ) const;
    void setSprinting( bool sprinting );

    // Estados da entidade
    void addEntityState( CBaseEntityState state );
    void removeEntityState( CBaseEntityState state );
    bool hasEntityState( CBaseEntityState state ) const;
    void clearEntityStates( );
    std::uint32_t getEntityStates( ) const;
    void setEntityStates( std::uint32_t states );

    // Hit e dano
    void Hit( int damage );
    bool isBeingHit( ) const;
    void stopBeingHit( );

    // Utilit�rios
    std::string getEntityStateAsString( ) const;
};
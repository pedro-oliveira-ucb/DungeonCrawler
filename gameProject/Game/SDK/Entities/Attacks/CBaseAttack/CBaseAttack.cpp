#include "CBaseAttack.h"

#include "../../../Utils/Log/Log.h"

CBaseAttack::CBaseAttack( CBaseEntityConstructor builder , CBaseAttackConstructor attackBuilder )
	: CBaseEntity( builder )
{
	this->damage = attackBuilder.damage;
	this->range = attackBuilder.range;
	this->duration = attackBuilder.duration;
	this->cooldown = attackBuilder.cooldown;
	this->delay = attackBuilder.delay;
	this->isActive = false;
	this->type = attackBuilder.type;
	this->speed = attackBuilder.speed;
}

CBaseAttack::CBaseAttack( const CBaseAttack & other )
	: CBaseEntity( other ) // Chama o construtor de cópia da classe base
{
	this->damage = other.damage;
	this->range = other.range;
	this->duration = other.duration;
	this->cooldown = other.cooldown;
	this->delay = other.delay;
	this->isActive = other.isActive;
	this->type = other.type;
	this->initialPos = other.initialPos;
	this->speed = other.speed;

	Log::Print( "[%s] Copy constructor called" , this->GetEntityName( ).c_str( ) );
}

void CBaseAttack::updateAttackPosition( ) {
	if ( isActive ) {
		float angleRad = this->getLookingAngle( ).getRadians( );

		GVector2D newDirection(
			cosf( angleRad ) * this->speed,
			sinf( angleRad ) * this->speed
		);

		GVector2D nextPosition = this->getEntityPosition( ) + newDirection;

		float positionDelta = GVector2D( this->initialPos - nextPosition ).length( );

		bool animationFinished = this->getEntityAnimations( )->getAnimationCycle( );

		if ( positionDelta > this->range || animationFinished ) {
			Log::Print( "[%s] Attack out of range" , this->GetEntityName( ).c_str( ) );
			if ( animationFinished ) {
				this->isActive = false;
				this->setEntityPosition( GVector2D( 0 , 0 ) );
			}
			return;
		}

		this->setEntityPosition( nextPosition );
		Log::Print( "[%s] Attack addPos: %f,%f, delta: %f, range: %f" ,
			this->GetEntityName( ).c_str( ) , newDirection.x , newDirection.y , positionDelta , this->range );
	}
}

bool CBaseAttack::IsActive( ) const {
	return isActive;
}

float CBaseAttack::GetDamage( ) const {
	return damage;
}

float CBaseAttack::GetRange( ) const {
	return range;
}

void CBaseAttack::Active( CBaseEntity * sender ) {
	this->isActive = true;
	this->setEntityPosition( sender->getEntityPosition( ) );
	this->setLookingAngle( sender->getLookingAngle( ).getDegrees() );
	this->setEntityLookingDirection( sender->getEntityLookingDirection( ) );
	this->setEntityMovementDirection( sender->getEntityMovementDirection( ) );


	this->initialPos = this->getEntityPosition();
}
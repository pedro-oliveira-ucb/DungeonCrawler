#include "CBaseAttack.h"

#include "../../../Events/EventManager.h"

#include "../../../Utils/Log/Log.h"


CBaseAttack::CBaseAttack( CBaseEntityConstructor builder , CBaseAttackConstructor attackBuilder )
	: CBaseEntity( builder )
{
	this->damage = attackBuilder.damage;
	this->range = attackBuilder.range;
	this->cooldown = attackBuilder.cooldown;
	this->delay = attackBuilder.delay;
	this->active = false;
	this->type = attackBuilder.type;
	this->speed = attackBuilder.speed;
	this->area = attackBuilder.area;
	this->Name = attackBuilder.Name;
}

CBaseAttack::CBaseAttack( const CBaseAttack & other )
	: CBaseEntity( other ) // Chama o construtor de cópia da classe base
{
	this->Name = other.Name;
	this->damage = other.damage;
	this->range = other.range;
	this->cooldown = other.cooldown;
	this->delay = other.delay;
	this->active = other.active;
	this->type = other.type;
	this->initialPos = other.initialPos;
	this->speed = other.speed;
	this->area = other.area;

	Log::Print( "[%s] Copy constructor called" , this->GetEntityName( ).c_str( ) );
}



std::shared_ptr<CBaseAttack> CBaseAttack::Clone() {
		std::shared_ptr<CBaseAttack> clone = std::make_shared<CBaseAttack>( *this ); // Usa cópia
		return clone;
}

void CBaseAttack::otherActiveLogic( CBaseEntity * sender ) {

}

void CBaseAttack::otherDeactiveLogic( ) {

}

void CBaseAttack::Active( CBaseEntity * sender ) {
	EventManager::Get( ).Trigger( this->GetEntityName() + "_attackThrow" );

	this->active = true;
	this->setLookingAngle( sender->getLookingAngle( ).getDegrees() );
	this->setEntityLookingDirection( sender->getEntityLookingDirection( ) );
	this->setEntityMovementDirection( sender->getEntityMovementDirection( ) );
	this->setEntityPosition( sender->getEntityPosition() );

	otherActiveLogic( sender );
}

void CBaseAttack::Deactive( ) {
	this->active = false;
	this->setEntityPosition( GVector2D( 0 , 0 ) );
	otherDeactiveLogic( );
}

bool CBaseAttack::IsActive( ) const {
	return this->active;
}

float CBaseAttack::getDamage( ) const {
	return this->damage;
}

float CBaseAttack::getRange( ) const {
	return this->range;
}

float CBaseAttack::getDelay( ) const {
	return this->delay;
}

float CBaseAttack::getCooldown( ) const {
	return this->cooldown;
}

float CBaseAttack::getSpeed( ) const {
	return this->speed;
}

GVector2D CBaseAttack::getArea( ) const {
	return this->area;
}

CBaseAttackType CBaseAttack::getAttackType( ) {
	return this->type;
}

std::string CBaseAttack::getName( ) const {
	return this->Name;
}

GVector2D CBaseAttack::getInitialPosition( ) const {
	return this->initialPos;
}

void CBaseAttack::setInitialPosition( GVector2D position ) {
	this->initialPos = position;
}
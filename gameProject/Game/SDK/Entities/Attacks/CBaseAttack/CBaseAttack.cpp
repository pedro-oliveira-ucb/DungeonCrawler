#include "CBaseAttack.h"

#include "../../../Events/EventManager.h"

#include "./../../../../Utils/Log/Log.h"


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

bool CBaseAttack::hasAlreadyHit( CBaseEntity * entity )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return hitEntities.find( entity ) != hitEntities.end( );
}

void CBaseAttack::registerHit( CBaseEntity * entity ) {
	{
		std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
		hitEntities.insert( entity );
	}
	EventManager::Get( ).Trigger( this->getName( ) + "_attackHit" );
}

void CBaseAttack::resetHits( ) {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	hitEntities.clear( );
}

std::shared_ptr<CBaseAttack> CBaseAttack::Clone( ) {
	std::shared_ptr<CBaseAttack> clone = std::make_shared<CBaseAttack>( *this ); // Usa cópia
	return clone;
}

void CBaseAttack::otherActiveLogic( CBaseEntity * sender ) {

}

void CBaseAttack::otherDeactiveLogic( ) {

}

void CBaseAttack::Active( CBaseEntity * sender ) {
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_attackThrow" );

	this->active = true;
	this->setLookingAngle( sender->getLookingAngle( ).getDegrees( ) );
	this->setEntityLookingDirection( sender->getEntityLookingDirection( ) );
	this->setEntityMovementDirection( sender->getEntityMovementDirection( ) );
	this->setEntityPosition( sender->getEntityPosition( ) );

	otherActiveLogic( sender );
}

void CBaseAttack::Deactive( ) {
	this->active = false;
	this->setEntityPosition( GVector2D( 0 , 0 ) );
	otherDeactiveLogic( );
}

bool CBaseAttack::IsActive( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->active;
}

float CBaseAttack::getDamage( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->damage;
}

float CBaseAttack::getRange( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->range;
}

float CBaseAttack::getDelay( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->delay;
}

float CBaseAttack::getCooldown( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->cooldown;
}

float CBaseAttack::getSpeed( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->speed;
}

GVector2D CBaseAttack::getArea( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->area;
}

CBaseAttackType CBaseAttack::getAttackType( ) const{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->type;
}

std::string CBaseAttack::getName( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	return this->Name;
}

void CBaseAttack::setRange( float range )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	this->range = range;
}

void CBaseAttack::setDamage( float damage )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	this->damage = damage;
}

void CBaseAttack::setCooldown( float cooldown )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	this->cooldown = cooldown;
}

void CBaseAttack::setDelay( float delay )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	this->delay = delay;
}

void CBaseAttack::setSpeed( float speed )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	this->speed = speed;
}

void CBaseAttack::setArea( GVector2D area )
{
	std::lock_guard<std::mutex> lock( this->cBaseAttackMutex );
	this->area = area;
}

GVector2D CBaseAttack::getInitialPosition( ) const {
	return this->initialPos;
}

void CBaseAttack::setInitialPosition( GVector2D position ) {
	this->initialPos = position;
}
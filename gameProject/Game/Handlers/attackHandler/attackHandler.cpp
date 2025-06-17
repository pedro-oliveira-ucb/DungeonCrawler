#include "attackHandler.h"
#include <mutex>

#include "../../gameObjects/gameMap/gameMap.h"

#include "../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include "../entitiesHandler/entitiesHandler.h"
#include "../../Managers/collisionManager/collisionManager.h"

#include "../../../Utils/Log/Log.h"

std::mutex attackHandlerMutex;

void attackHandler::updateAttacks( )
{
	std::lock_guard<std::mutex> lock( attackHandlerMutex );

	auto it = runningAttacks.begin( );
	while ( it != runningAttacks.end( ) )
	{
		CBaseEntity * sender = it->first;
		CBaseAttack * attack = it->second.get( );

		if ( attack == nullptr || sender == nullptr )
		{
			it = runningAttacks.erase( it ); // retorna o novo iterador válido
		}
		else
		{
			if ( attack->IsActive( ) ) {

				GVector2D attackPosition = attack->getEntityPosition( );

				if ( !gameMap::Get( ).inInMap( attackPosition ) ) {
					gameSoundsEventHandler::Get( ).addEventToQueue( attack->GetEntityName( ) + "_hitWall" );
					it = runningAttacks.erase( it );
					continue;
				}

				std::vector<CBaseEntity *> targets = CollisionManager::Get( ).GetAllEntities( );

				if ( !targets.empty( ) ) {
					for ( int i = 0; i < targets.size( ); i++ ) {

						if ( targets.at( i ) == nullptr )
							continue;

						if ( !targets.at( i )->isAlive( ) )
							continue;

						if ( targets.at( i ) == sender )
							continue;

						if ( targets.at( i )->getEntityType( ) == sender->getEntityType( ) )
							continue;

						// Se já foi atingida, ignora
						if ( attack->hasAlreadyHit( targets.at( i ) ) )
							continue;

						if ( CollisionManager::Get( ).checkCollision( attack , targets.at( i ) , attackPosition ) ) {
							targets.at( i )->Hit( ( int ) attack->getDamage( ) );
							attack->registerHit( targets.at( i ) );
						}
					}
				}

				attack->updateEntity( );
			}
			else
			{
				it = runningAttacks.erase( it ); // retorna o novo iterador válido
				continue;
			}

			++it;
		}
	}
}

int attackHandler::runningAttacksSize( )
{
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	return runningAttacks.size( );
}

std::shared_ptr<CBaseAttack> attackHandler::getRunningAttack( int index )
{
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	if ( index >= 0 && index < runningAttacks.size( ) )
	{
		return runningAttacks.at( index ).second;
	}
	return nullptr;
}

void attackHandler::throwNewAttack( CBaseEntity * sender , CBaseAttack * attack )
{
	std::shared_ptr<CBaseAttack> newAttack = attack->Clone( );
	newAttack->Active( sender );
	{
		std::lock_guard<std::mutex> lock( attackHandlerMutex );
		runningAttacks.push_back( std::make_pair( sender , newAttack ) );
	}
}

void attackHandler::addAvailableLocalPlayerAttack( std::shared_ptr<CBaseAttack> attack ) {
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	this->availableLocalPlayerAttacks.emplace( attack->getAttackType( ) , std::move( attack ) );
}

void attackHandler::addAvailableEnemyAttack( std::string enemyName , std::shared_ptr<CBaseAttack> attack ) {
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	this->availableEnemiesAttacks[ enemyName ].emplace_back( availableAttackHolder( attack->getAttackType( ) , attack ) );
}

void attackHandler::clearAttacks( )
{
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	runningAttacks.clear( );
}

std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> * attackHandler::getAvailableLocalPlayerAttack( ) {
	//run this once, and store it
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	return &this->availableLocalPlayerAttacks;
}

std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>>  attackHandler::getAvailableEnemyAttack( std::string enemyName ) {
	//run this once, and store it
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> result;
	auto it = this->availableEnemiesAttacks.find( enemyName );
	if ( it != this->availableEnemiesAttacks.end( ) ) {
		//run this once, and store it	
		for ( int i = 0; i < it->second.size( ); i++ ) {
			availableAttackHolder * attack = &it->second.at( i );
			result.emplace( std::make_pair( attack->attack->getAttackType( ) , attack->attack ) );
		}
	}

	return result;
}
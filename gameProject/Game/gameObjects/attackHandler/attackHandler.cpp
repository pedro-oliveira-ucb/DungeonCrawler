#include "attackHandler.h"
#include <mutex>

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
				attack->updateAttackPosition( );
				attack->getEntityAnimations( )->updateAnimation( false );
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

std::shared_ptr<CBaseAttack>  attackHandler::throwNewAttack( CBaseEntity * sender , CBaseAttack * attack )
{
	std::lock_guard<std::mutex> lock( attackHandlerMutex );
	std::shared_ptr<CBaseAttack> newAttack = attack->Clone( );
	newAttack->Active( sender );
	runningAttacks.push_back( std::make_pair( sender , newAttack ) );

	return newAttack;
}
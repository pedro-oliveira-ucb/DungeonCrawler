#include "LevelHandler.h"

void LevelHandler::addLevel( mapType type , gameRoomLevel level )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	this->m_levels[ type ].push_back( level );
}

void LevelHandler::clearLevels( )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	this->m_levels.clear( );
}

std::vector<gameRoomLevel> * LevelHandler::getMapRoomLevels( mapType type )
{
	std::lock_guard<std::mutex> lock( m_mutex );
	auto it = this->m_levels.find( type );
	if ( it != this->m_levels.end() )
	{
		return &it->second;
	}
	return nullptr;
}

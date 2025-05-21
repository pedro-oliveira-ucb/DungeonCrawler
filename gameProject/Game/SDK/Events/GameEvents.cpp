// GameEvents.cpp
#include "GameEvents.h"

void GameEvents::on( const std::string & eventName , Callback callback ) {
    listeners[ eventName ].push_back( callback );
}

void GameEvents::emit( const std::string & eventName ) {
    if ( listeners.find( eventName ) != listeners.end( ) ) {
        for ( const auto & callback : listeners[ eventName ] ) {
            callback( );
        }
    }
}

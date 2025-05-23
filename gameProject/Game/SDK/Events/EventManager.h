#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex> // Adicionado para controle de concorrência
#include "GameEvents.h"

#include "../../../Utils/singleton.h"

class EventManager : public CSingleton<EventManager> {
    std::unordered_map<std::string , std::vector<std::shared_ptr<IGameEvent>>> eventMap;
    std::mutex mutex; // Mutex para proteger o acesso ao eventMap

public:
    void RegisterEvent( const std::string & trigger , std::shared_ptr<IGameEvent> event ) {
        std::lock_guard<std::mutex> lock( mutex );
        eventMap[ trigger ].push_back( std::move( event ) );
    }

    void Trigger( const std::string & trigger ) {
        std::vector<std::shared_ptr<IGameEvent>> eventsToExecute;
        std::lock_guard<std::mutex> lock( mutex );
        {           
            if ( eventMap.count( trigger ) ) {
                eventsToExecute = eventMap[ trigger ]; // Copia os eventos para executar fora do lock
            }
        }

        // Executa os eventos fora do mutex para evitar deadlocks
        for ( auto & event : eventsToExecute ) {
            event->Execute( );
        }
    }
};

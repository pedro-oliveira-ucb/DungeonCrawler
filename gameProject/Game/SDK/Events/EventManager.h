#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include "GameEvents.h"

#include "../../../Utils/singleton.h"

class EventManager : public CSingleton<EventManager>{
    std::unordered_map<std::string, std::vector<std::shared_ptr<IGameEvent>>> eventMap;
public:
    void RegisterEvent(const std::string& trigger, std::shared_ptr<IGameEvent> event) {
        eventMap[trigger].push_back(event);
    }

    void Trigger(const std::string& trigger) {
        if (eventMap.count(trigger)) {
            for (auto& event : eventMap[trigger]) {
                event->Execute();
            }
        }
    }
};
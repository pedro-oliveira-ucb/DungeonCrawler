// GameEvents.h
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

class GameEvents {
public:
    using Callback = std::function<void( )>;

    void on( const std::string & eventName , Callback callback );
    void emit( const std::string & eventName );

private:
    std::unordered_map<std::string , std::vector<Callback>> listeners;
};

#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>
#include "../../../Utils/singleton.h"

enum keybind_mode {
	HOLD ,
	TOGGLE
};

enum keybind_identifier {
	MOVE_FORWARD ,
	MOVE_BACKWARD ,
	MOVE_LEFT ,
	MOVE_RIGHT ,
	ATTACK_SIMPLE ,
	ATTACK_HEAVY ,
	SPRINT,
	UPDATE_MENU
};

class keybind {
public:
	int key;
	int mode = keybind_mode::HOLD;

	keybind( ) : key( 0 ) , mode( keybind_mode::HOLD ) { }
	keybind( int k , int m ) : key( k ) , mode( m ) { }
};

class keybindHandler : public CSingleton<keybindHandler> {
	std::mutex keybindUpdateMutex;

	std::unordered_map<keybind_identifier , keybind> keybinds;
	std::unordered_map<keybind_identifier , bool> pressedKeys;
	std::unordered_map<keybind_identifier , bool> toggleStates;
	std::unordered_map<keybind_identifier , bool> lastKeyDown;

public:
	bool isPressed( keybind_identifier identifier );
	void update( );
	bool initializeKeybinds( );
};

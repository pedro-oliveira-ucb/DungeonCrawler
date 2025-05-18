#pragma once
#include <vector>
#include <unordered_map>
#include "../../../Utils/singleton.h"

enum keybind_mode {
	HOLD ,
	TOGGLE
};

enum keybind_identifier {
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	ATTACK_SIMPLE,
	ATTACK_HEAVY,
	UPDATE_MENU
};

class keybind {
public:
	int key;
	int mode = keybind_mode::HOLD;

	keybind( ) : key( 0 ) , mode( keybind_mode::HOLD ) {} // Construtor padr�o
	keybind( int k , int m ) : key( k ) , mode( m ) {}
};

class keybindHandler : public CSingleton< keybindHandler>
{
	std::unordered_map<keybind_identifier , keybind> keybinds;

public:
	
	bool isPressed( keybind_identifier identifier );
	bool initializeKeybinds( );
};


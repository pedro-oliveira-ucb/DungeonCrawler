#include "keybindHandler.h"

#include "../../../Utils/Log/Log.h"

#include <Windows.h>


std::unordered_map<keybind_identifier , bool> toggleStates;
std::unordered_map<keybind_identifier , bool> lastKeyDown;

bool keybindHandler::isPressed( keybind_identifier id ) {
	auto it = keybinds.find( id );
	if ( it == keybinds.end( ) ) return false;

	keybind & kb = it->second;
	SHORT state = GetAsyncKeyState( kb.key );
	bool isDown = ( state & 0x8000 ) != 0;

	if ( kb.mode == keybind_mode::HOLD ) {
		return isDown;
	}
	else { // TOGGLE
		bool wasDown = lastKeyDown[ id ];
		if ( isDown && !wasDown ) {
			// borda de descida: invertendo o estado
			toggleStates[ id ] = !toggleStates[ id ];
		}
		lastKeyDown[ id ] = isDown;
		return toggleStates[ id ];
	}
}

bool keybindHandler::initializeKeybinds( ) {
	this->keybinds = {
		{ MOVE_FORWARD,  keybind( 'W', keybind_mode::HOLD ) },
		{ MOVE_BACKWARD, keybind( 'S', keybind_mode::HOLD ) },
		{ MOVE_LEFT,     keybind( 'A', keybind_mode::HOLD ) },
		{ MOVE_RIGHT,    keybind( 'D', keybind_mode::HOLD ) },
		{ ATTACK_SIMPLE, keybind( VK_LBUTTON , keybind_mode::HOLD ) },
		{ ATTACK_HEAVY,  keybind( VK_RBUTTON , keybind_mode::HOLD ) },
		{ UPDATE_MENU,    keybind( VK_DELETE, keybind_mode::TOGGLE ) }
	};
	return true;
}
#include "keybindHandler.h"
#include "../../../Utils/Log/Log.h"
#include <raylib/raylib.h>

bool keybindHandler::isPressed( keybind_identifier id ) {
	std::lock_guard<std::mutex> lock( this->keybindUpdateMutex );
	return this->pressedKeys[ id ];
}

void keybindHandler::update( ) {
	std::lock_guard<std::mutex> lock( this->keybindUpdateMutex );

	for ( auto & [id , kb] : this->keybinds ) {
		bool isDown = ( kb.key >= MOUSE_LEFT_BUTTON && kb.key <= MOUSE_RIGHT_BUTTON )
			? IsMouseButtonDown( kb.key )
			: IsKeyDown( kb.key );


		if ( kb.mode == keybind_mode::HOLD ) {
			this->pressedKeys[ id ] = isDown;
		}
		else { // TOGGLE
			bool justPressed = ( kb.key >= MOUSE_LEFT_BUTTON && kb.key <= MOUSE_RIGHT_BUTTON )
				? IsMouseButtonPressed( kb.key )
				: IsKeyPressed( kb.key );


			bool wasDown = lastKeyDown[ id ];
			if ( justPressed ) {
				toggleStates[ id ] = !toggleStates[ id ];
			}
			lastKeyDown[ id ] = isDown;
			this->pressedKeys[ id ] = toggleStates[ id ];
		}
	}
}

bool keybindHandler::initializeKeybinds( ) {
	std::lock_guard<std::mutex> lock( this->keybindUpdateMutex );

	this->keybinds = {
		{ MOVE_FORWARD,  keybind( 'W', keybind_mode::HOLD ) },
		{ MOVE_BACKWARD, keybind( 'S', keybind_mode::HOLD ) },
		{ MOVE_LEFT,     keybind( 'A', keybind_mode::HOLD ) },
		{ MOVE_RIGHT,    keybind( 'D', keybind_mode::HOLD ) },
		{ SPRINT,    keybind( KEY_LEFT_SHIFT, keybind_mode::HOLD ) },
		{ ATTACK_SIMPLE, keybind( MOUSE_LEFT_BUTTON, keybind_mode::HOLD ) },
		{ ATTACK_HEAVY,  keybind( MOUSE_RIGHT_BUTTON, keybind_mode::HOLD ) },
		{ UPDATE_MENU,   keybind( KEY_DELETE, keybind_mode::TOGGLE ) }
	};

	// Inicializar estados de toggle
	for ( const auto & [id , _] : this->keybinds ) {
		this->toggleStates[ id ] = false;
		this->lastKeyDown[ id ] = false;
		this->pressedKeys[ id ] = false;
	}

	return true;
}

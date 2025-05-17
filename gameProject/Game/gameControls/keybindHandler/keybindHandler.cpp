#include "keybindHandler.h"

#include <Windows.h>

bool keybindHandler::isPressed( keybind_identifier identifier ) {
    if ( this->keybinds.find( identifier ) != this->keybinds.end( ) ) {
        keybind key = this->keybinds[ identifier ];
        switch ( key.mode ) {
        case keybind_mode::HOLD:
            return ( GetAsyncKeyState( key.key ) & 0x8000 );
        case keybind_mode::TOGGLE:
            return ( GetAsyncKeyState( key.key ) & 0x1 );
        }
    }
    return false;
}

bool keybindHandler::initializeKeybinds( ) {
    this->keybinds = {
        { MOVE_FORWARD,  keybind( (int)'w', keybind_mode::HOLD ) },
        { MOVE_BACKWARD, keybind( ( int ) 's', keybind_mode::HOLD ) },
        { MOVE_LEFT,     keybind( ( int ) 'a', keybind_mode::HOLD ) },
        { MOVE_RIGHT,    keybind( ( int ) 'd', keybind_mode::HOLD ) },
        { UPDATE_MENU,    keybind( VK_DELETE, keybind_mode::HOLD ) }
    };
    return true;
}
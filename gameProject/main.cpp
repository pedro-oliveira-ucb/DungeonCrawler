#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>

#include "Game/game.h"
#include "Game/gameRender/gameRender.h"
#include "Globals/Globals.h"
#include "Utils/Log/Log.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

Game game;

void idle( ) {
	Log::Print( "[main] Entering idle!" );
	while ( !globals.exitGame ) {
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
	Log::Print( "[main] Exiting game!" );
}

void waitGameLoad( ) {
	while ( !game.isResourcesLoaded( ) ) {
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
}

sf::Font g_font( "arial.ttf" );

int main( void ) {

	Log::Print( "[Render] Initialized window!" );



	//sf::SoundBuffer buffer;
	//buffer.loadFromStream( stream );

	//// Display sound information
	//std::cout << "killdeer.wav:" << '\n'
	//	<< " " << buffer.getDuration( ).asSeconds( ) << " seconds" << '\n'
	//	<< " " << buffer.getSampleRate( ) << " samples / sec" << '\n'
	//	<< " " << buffer.getChannelCount( ) << " channels" << '\n';

	//// Create a sound instance and play it
	//sf::Sound sound( buffer );
	//sound.play( );

	//// Loop while the sound is playing
	//while ( sound.getStatus( ) == sf::Sound::Status::Playing )
	//{
	//	// Leave some CPU time for other processes
	//	sf::sleep( sf::milliseconds( 100 ) );

	//	// Display the playing position
	//	std::cout << "\rPlaying... " << sound.getPlayingOffset( ).asSeconds( ) << " sec        " << std::flush;
	//}

	//std::cout << '\n' << std::endl;

	Log::Print( "[main] starting game!" );
	game.start( );
	Log::Print( "[main] game started!" );

	waitGameLoad( );

	// Create the main window
	sf::RenderWindow window( sf::VideoMode( { 1920, 1080 } ) , "SFML window" );
	globals.windowPointer = &window;

	// Start the game loop
	while ( window.isOpen( ) )
	{
		// Process events
		while ( const std::optional event = window.pollEvent( ) )
		{
			// Close window: exit
			if ( event->is<sf::Event::Closed>( ) )
				window.close( );

			// Detecta scroll do mouse
			if ( event->is<sf::Event::MouseWheelScrolled>( ) ) {
				globals.mouseWheelDelta += event->getIf<sf::Event::MouseWheelScrolled>( )->delta;
			}
		}

		gameRender::Get( ).soundEvents( );

		auto mousePos = sf::Mouse::getPosition( );

		globals.mousePosX = mousePos.x;
		globals.mousePosY = mousePos.y;

		gameRender::Get( ).render( window );

		// Clear screen
		window.clear( );

		// Update the window
		window.display( );
	}

	window.close( );
	idle( );
	return 1;
}
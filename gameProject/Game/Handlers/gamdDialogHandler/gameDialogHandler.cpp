#include "gameDialogHandler.h"

void gameDialogHandler::throwDialog( gameDialog dialog ) {
	std::lock_guard<std::mutex> lock( this->dialogMutex ); // Lock the mutex to ensure thread safety

	this->dialogQueue.push( dialog );
}

gameDialog gameDialogHandler::getDialog() {
	std::lock_guard<std::mutex> lock( this->dialogMutex ); // Lock the mutex to ensure thread safety
	if ( this->dialogQueue.empty() ) {
		return gameDialog(); // Return an empty dialog if the queue is empty
	}
	gameDialog dialog = this->dialogQueue.front();
	this->dialogQueue.pop();
	return dialog;
}

bool gameDialogHandler::hasActiveDialog() {
	std::lock_guard<std::mutex> lock( this->dialogMutex ); // Lock the mutex to ensure thread safety
	return !this->dialogQueue.empty( ); // Return true if there is an active dialog
}

void gameDialogHandler::clearDialogs( )
{
	std::lock_guard<std::mutex> lock( this->dialogMutex ); // Lock the mutex to ensure thread safety
	while ( !this->dialogQueue.empty( ) ) {
		this->dialogQueue.pop( ); // Clear all dialogs in the queue
	}
}

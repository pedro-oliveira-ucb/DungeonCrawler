#pragma once
#include <string>
#include <queue>
#include <mutex>

#include "../../../Utils/singleton.h"

struct timedTrigger {
	std::string triggerName; // The name of the trigger
	int triggerTime; // The time at which the trigger should activate in seconds
	bool throwed = false;

	timedTrigger( const std::string & trigger, const int & time ): triggerName(trigger), triggerTime(time){}
};

struct gameDialog {
	std::string dialogText; // The text of the dialog
	int dialogDuration = 0;
	int dialogStayTime = 0; // The time the dialog should stay on screen after end
	std::vector<timedTrigger> triggers; // A queue of timed triggers associated with the dialog

	gameDialog( const std::string & text , const int & duration, const std::vector<timedTrigger> & trig = {} )
		: dialogText( text ) , dialogDuration(duration), triggers( trig ) {
	}	
	gameDialog( ) {
		this->dialogText = "";
		this->dialogDuration = 0;
	}

	bool empty( ) {
		return this->dialogText.empty( ) && this->dialogDuration == 0 && this->triggers.empty( );
	}
	void clear() {
		this->dialogText.clear( );
		this->dialogDuration = 0;
		this->triggers.clear( );
	}
};

class gameDialogHandler : public CSingleton<gameDialogHandler>  // Singleton pattern to ensure only one instance of gameDialogHandler exists
{
	std::mutex dialogMutex; // Mutex to protect access to the dialog queue
	std::queue<gameDialog> dialogQueue; // Queue to hold dialogs that are waiting to be displayed
public:
	bool hasActiveDialog( );
	void clearDialogs( );

	gameDialog getDialog( ); // Function to retrieve the next dialog from the queue
	void throwDialog( gameDialog dialog ); // Function to display a dialog with its associated triggers
};


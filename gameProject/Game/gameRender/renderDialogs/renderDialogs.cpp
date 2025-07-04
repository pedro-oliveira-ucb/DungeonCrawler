#include "renderDialogs.h"

#include "../../SDK/Events/EventManager.h"

#include "../../../Globals/Globals.h"

dialogBox renderDialogs::renderDialogBox( std::string text , Font font ) {
	dialogBox box;
	box.text = text;
	box.fontSize = 20;
	box.textFont = font;

	Vector2 textSize = MeasureTextEx( font , box.text.c_str( ) , box.fontSize , 0 );
	box.textWidth = textSize.x;
	box.textHeight = textSize.y;

	box.padding = 20.f; // Padding around the text
	// 20px [text] 20px
	box.width = box.textWidth + ( box.padding * 2 );
	box.height = 50.f;

	box.y = Globals::Get( ).screenHeight - box.height * 5;
	// Calculate the X position to center the box
	box.x = ( Globals::Get().screenWidth / 2 ) - ( box.width / 2 );
	DrawRectangle( box.x , box.y , box.width , box.height , BLACK );
	return box;
}

void renderDialogs::renderDialogText( dialogBox box ) {

	const char * textToDraw = box.text.c_str( );
	float textPositionX = box.x + box.padding;
	float textPositionY = box.y + ( box.height / 2 ) - box.textHeight / 2;

	Vector2 textPosition = { textPositionX ,  textPositionY };

	DrawTextEx( box.textFont , textToDraw , textPosition , box.fontSize , 0 , WHITE );
}

void renderDialogs::render( ) {

	if ( this->currentDialog.empty( ) ) {
		this->currentDialog = gameDialogHandler::Get( ).getDialog( );
	}
	else {
		float currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );

		if ( !this->dialogStartTime ) {
			this->dialogStartTime = currentGameTime;
		}

		this->currentDialogTime = currentGameTime - this->dialogStartTime;

		for ( auto it = this->currentDialog.triggers.begin( ); it != this->currentDialog.triggers.end( ); ++it ) {
			if ( this->currentDialogTime >= it->triggerTime && !it->throwed ) {
				EventManager::Get( ).Trigger( it->triggerName );
				it->throwed = true;
			}
		}
		float lettersPerSecond = ( this->currentDialog.dialogText.size( ) / this->currentDialog.dialogDuration );
		static int oldLettersToShow = 0;
		int lettersToShow = static_cast< int >( this->currentDialogTime * lettersPerSecond );

		if ( lettersToShow > this->currentDialog.dialogText.size( ) ) {
			lettersToShow = this->currentDialog.dialogText.size( );
			if ( !this->dialogEndTime ) {
				this->dialogEndTime = currentGameTime;
			}

			float deltaEndTime = currentGameTime - this->dialogEndTime;
			if ( deltaEndTime > this->currentDialog.dialogStayTime ) {
				// Reset the dialog after it has been fully displayed
				this->currentDialog.clear( );
				this->currentDialogTime = 0;
				this->dialogStartTime = 0;
				this->dialogEndTime = 0;
				return;
			}
		}
		else if( oldLettersToShow != lettersToShow) {
			EventManager::Get( ).Trigger( "dialog_Update" );
			oldLettersToShow = lettersToShow;
		}

		
		std::string textToRender = this->currentDialog.dialogText.substr( 0 , lettersToShow );
		dialogBox box = renderDialogBox( textToRender , GetFontDefault( ) );
		renderDialogText( box );
	}
}
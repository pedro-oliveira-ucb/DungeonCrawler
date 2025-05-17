#include "CBaseThread.h"

#include <atomic>
#include <string>
#include <mutex>
#include <vector>

std::vector<bool> threadsReady;
std::mutex threadReadyMutex;

void CBaseThread::start( ) {
	ThreadObject = std::make_unique<Thread>( ( LPTHREAD_START_ROUTINE ) threadFunctionWrapper , this , true );
	if ( !ThreadObject->GetHandle( ) || ThreadObject->GetHandle( ) == INVALID_HANDLE_VALUE ) {
	
	}
}

void CBaseThread::reset( ) {
	stop( );
	start( );
}

void CBaseThread::threadFunctionWrapper( LPVOID instance ) {
	// static_cast< ThreadHolder * >( instance )->waitOtherThreads( );
	static_cast< CBaseThread * >( instance )->threadFunction( );
}

void CBaseThread::stop( ) {
	if ( ThreadObject ) {
		ThreadObject->SignalShutdown( true );
	}
}


#pragma once

#include <thread>
#include "../../../Process/Thread.hpp"

class CBaseThread {
protected:
   
    static void threadFunctionWrapper( LPVOID instance );
   
private:
    int SleepTime = 10;

    virtual void threadFunction( ) = 0;
public:
  
    std::unique_ptr<Thread> ThreadObject;

    virtual ~CBaseThread( ) { stop( ); }

    virtual void start( );

    virtual void stop( );

    virtual void reset( );

    virtual int getThreadSleepTime( ) const { return SleepTime; }  // Default sleep time
};
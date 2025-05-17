#include "Log.h"

#include <cstdarg>
#include <cstdio>

#include <mutex>

std::mutex logMutex;

void Log::Print( const char * format , ... ) {
    //One log at time
    std::lock_guard<std::mutex> lock( logMutex );

    va_list args;
    va_start( args , format );

    // Você pode interceptar a mensagem antes de exibir
    std::vprintf( format , args );
    std::printf( "\n" );

    va_end( args );
}

void Log::Error( const char * format , va_list args ) {
   
}
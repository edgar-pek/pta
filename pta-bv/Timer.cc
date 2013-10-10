#include "Timer.h"

int Timer::timerId = 0;

void Timer::start( )
{
	//cerr << "timer " << this->id << " started.\n";
	this->startTime = cpuTime( );
}

double Timer::elapsed( )
{
	return cpuTime( ) - this->startTime;
}

void Timer::printElapsed( )
{
	cerr << "@timer " << this->id << ": " << this->elapsed( ) << endl;
}

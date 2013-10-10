#ifndef TIMER_H
#define TIMER_H

#include "global.h"
#include <sys/time.h>
#include <sys/resource.h>

static inline double cpuTime_us(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec ;
}

static inline double cpuTime_ms(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000;
}

static inline double cpuTime(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
}

class Timer
{
public:
	Timer( ) { ++timerId; this->id = timerId; }
	void   start  ( );
	double elapsed( );
	void   printElapsed( );
private:
	static int timerId;
	int id;
	double startTime;

};

#endif

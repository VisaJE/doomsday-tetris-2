#include "ProcessTimer.h"
#include "Log.h"
#include <iostream>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace tet;

ProcessTimer::ProcessTimer(unsigned repeats, std::string name):
    repeats(repeats), counter(0), name(name), time(0.0)
{
    refTime = clock();
}

ProcessTimer::ProcessTimer(unsigned repeats):
    repeats(repeats), counter(0), name("Timer"), refTime(0.0), time(0.0)
{
}

void ProcessTimer::tick()
{
    if (counter++ >= repeats)
    {
        auto timeN = clock();
        time = (double) timeN- refTime;
        refTime = timeN;
        counter = 0;
    }
}

double ProcessTimer::getTimeSeconds()
{
    return time/CLOCKS_PER_SEC;
}

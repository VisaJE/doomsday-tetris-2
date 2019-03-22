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

ProcessTimer::~ProcessTimer()
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
        LOG("Timer %s counted time %f", name.c_str(), (double)time/CLOCKS_PER_SEC);
    }
}


void ProcessTimer::start()
{
    refTime = clock();
}
void ProcessTimer::end()
{
    auto tN = clock();
    time += tN-refTime;
    if (counter ++ >= repeats)
    {
        LOG("Timer %s counted time %f", name.c_str(), (double)time/CLOCKS_PER_SEC);
        time = 0;
        counter = 0;
    }
}

double ProcessTimer::getTimeSeconds()
{
    return (double)time/CLOCKS_PER_SEC;
}

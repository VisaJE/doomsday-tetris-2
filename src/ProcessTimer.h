#pragma once
#include "Log.h"
#include <iostream>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

namespace tet {

class ProcessTimer
{
    public:
        ProcessTimer(unsigned repeats, std::string name);
        ProcessTimer(unsigned repeats);
        virtual ~ProcessTimer();
        void tick();
        double getTimeSeconds();
    private:
        unsigned repeats;
        unsigned counter;
        std::string name;
        clock_t refTime;
        double time;

};

}

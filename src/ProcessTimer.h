#pragma once
#include "Log.h"
#include <iostream>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

namespace tet {

class ProcessTimer
{
    public:
        ProcessTimer(unsigned repeats, std::string name);
        virtual ~ProcessTimer();
        void tick();
        void start();
        void end();
        double getTimeSeconds();
    private:
        unsigned repeats;
        unsigned counter;
        std::string name;
        clock_t refTime;
        clock_t time;

};

}

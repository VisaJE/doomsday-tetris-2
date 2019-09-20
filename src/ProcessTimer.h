#pragma once
#include "Log.h"
#include <iostream>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

namespace tet {

class ProcessTimer
{
    public:
        ProcessTimer()
        {
            refTime = 0;
        }
    
        virtual ~ProcessTimer()
        {
        }

        void tick()
        {
            refTime = clock();
        }
        double getTimeSeconds()
        {
            auto timeN = clock();
            LOG("Got time %f\n", (double) (timeN- refTime)/CLOCKS_PER_SEC);
            return (double) (timeN- refTime)/CLOCKS_PER_SEC;
        }
    private:
        clock_t refTime;

};

}

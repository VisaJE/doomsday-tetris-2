#pragma once

#include <iostream>
#include "SqlConn.h"
#include "WebConf.h"
#include "Events.h"

namespace tet
{
class GlobalHighscore
{
    public:
        GlobalHighscore(Events& context);
        virtual ~GlobalHighscore();
        bool success = false;
        std::string names[10];
        int scores[10];
    private:
        Events& context;
        WebConf configuration;
};
}

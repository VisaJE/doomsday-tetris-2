#pragma once

#include <iostream>
#include "SqlConn.h"
#include "WebConf.h"
#include "Highscorer.h"

namespace tet
{
class GlobalHighscore
{
    public:
        GlobalHighscore(Highscorer *context);
        virtual ~GlobalHighscore();
        bool updateData();
        std::string names[10] = {""};
        int scores[10] = {0};
    private:
        Highscorer* context;
        WebConf configuration;
};
}

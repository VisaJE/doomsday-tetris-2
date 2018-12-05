#include <iostream>
#include "GlobalHighscore.h"
#include "SqlConn.h"
#include "WebConf.h"
#include <unistd.h>

using namespace tet;
GlobalHighscore::GlobalHighscore(Events& context): context(context)
{
    configuration = WebConf();
    configuration.initiate();
    if (!configuration.serviceEnabled)
    {
        cout << "Online highscore service not enabled" << endl;
        return;
    }
    SqlConn connection = SqlConn(configuration.configuration);
    //cout << "Made SqlConn" << connection.connectionStatus << endl;
}

GlobalHighscore::~GlobalHighscore()
{
}


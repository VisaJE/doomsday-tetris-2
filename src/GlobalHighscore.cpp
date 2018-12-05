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
    cout << "Made SqlConn" << connection.connectionStatus << endl;
    int scores[10];
    std::string names[10];
    connection.topList(names, scores);
    for (int i = 0; i < 10; i++)
    {
        cout << names[i] << ": " << scores[i] <<endl;
    }
}

GlobalHighscore::~GlobalHighscore()
{
}


#include <iostream>
#include "GlobalHighscore.h"
#ifndef NPOSTGRES
#include "SqlConn.h"
#include "WebConf.h"
#endif
#include <unistd.h>
#include "Highscorer.h"
#include "UniqueIdentifier.h"
#include "Log.h"

using namespace tet;
GlobalHighscore::GlobalHighscore(Highscorer *context): context(context)
{
}

bool GlobalHighscore::updateData()
{
#ifdef NPOSTGRES
    names[0] = "Online";
    names[1] = "Services";
    names[2] = "Not";
    names[3] = "Compiled!";
    names[5] = "";
    names[6] = "";
    names[7] = "";
    names[8] = "";
    return false;
#else
    configuration = WebConf();
    configuration.initiate();
    if (!configuration.serviceEnabled)
    {
        names[0] = "Online";
        names[1] = "Services";
        names[2] = "Are turned";
        names[3] = "Off!";
        names[5] = "The server";
        names[6] = "_config";
        names[7] = "File not";
        names[8] = "configured";
        return false;
    }
    SqlConn connection = SqlConn(configuration.configuration);
    int tscores[10];
    std::string tnames[10];
    Uid ids[10];
    context->getHighscore(tnames, tscores, ids);

    connection.pushList(tnames, tscores, ids);

    connection.topList(names, scores);

    return true;
#endif
}

GlobalHighscore::~GlobalHighscore()
{
}


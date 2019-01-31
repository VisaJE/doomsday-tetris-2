#include <iostream>
#include "GlobalHighscore.h"
#include "SqlConn.h"
#include "WebConf.h"
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
    // Highscorer ghs = Highscorer(".ghs.json");
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
    LOG("Getting highscore from server\nInfo before: \n%s %i\n \
             %s %i\n  %s %i\n  %s %i\n  %s %i\n  %s %i\n\
             %s %i\n  %s %i\n  %s %i\n  %s %i\n",
            names[0].c_str(), scores[0],
            names[1].c_str(), scores[1],
            names[2].c_str(), scores[2],
            names[3].c_str(), scores[3],
            names[4].c_str(), scores[4],
            names[5].c_str(), scores[5],
            names[6].c_str(), scores[6],
            names[7].c_str(), scores[7],
            names[8].c_str(), scores[8],
            names[9].c_str(), scores[9]
            );
    context->getHighscore(tnames, tscores, ids);

    LOG("Pushing highscore to server\n");
    connection.pushList(tnames, tscores, ids);

    LOG("New list from server\n");
    connection.topList(names, scores);

    return true;
}

GlobalHighscore::~GlobalHighscore()
{
}


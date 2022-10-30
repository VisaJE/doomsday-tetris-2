/*
 * Conf.cpp
 *
 *  Created on: 31.5.2018
 *      Author: Eemil
 */

#include "Conf.h"
#include "Paths.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <cstdio>
#include <iostream>

using namespace rapidjson;

#ifdef WIN32
#define READ_ACCES_SPECIFIER "rb"
#define WRITE_ACCES_SPECIFIER "wb"
#else
#define READ_ACCES_SPECIFIER "r"
#define WRITE_ACCES_SPECIFIER "w"
#endif

namespace tet
{
// Default values are set at the constructor.
Conf::Conf()
{
    const char norm[] = " { \"BOARD_WIDTH\": 10 , \"BOARD_HEIGHT\": 19, \"START_INTERVAL\": 500, "
                        "\"SCREEN_WIDTH\": 600, \"SCREEN_HEIGHT\": 900, \"SLIDE_SPEED\": 5 } ";
    Document defa;
    defa.Parse(norm);
    FILE *config = fopen(Paths::configPath().c_str(), READ_ACCES_SPECIFIER);
    if (!config)
    {
        LOG("No config\n");
        fallBack(&defa);
        setFromFile(&defa);
    }
    else
    {
        char readBuffer[65536];
        FileReadStream in(config, readBuffer, sizeof(readBuffer));
        Document found;
        found.ParseStream(in);
        if (!checkValidity(&found))
        {
            LOG("Non valid config");
            fclose(config);
            fallBack(&defa);
            setFromFile(&defa);
        }
        else
        {
            LOG("Config read");
            setFromFile(&found);
            fclose(config);
        }
    }
}

Conf::~Conf() {}

bool Conf::competitionValid()
{
    if (startInterval > 500)
        return false;
    if (slideSpeed > 8)
        return false;
    if (!(boardWidth == 10 && boardHeight == 19))
        return false;
    return true;
}

bool Conf::checkValidity(Document *d)
{
    if (!d->IsObject())
        return false;
    // Lets check the keys with int values.
    for (int i = 0; i < 6; i++)
    {
        Value::MemberIterator here = d->FindMember(keys[i]);
        if (here == d->MemberEnd())
            return false;
        if (!(here->value.IsNumber()) || here->value.GetInt() <= 0)
            return false;
    }
    return true;
}

void Conf::fallBack(rapidjson::Document *defa)
{
    FILE *config = fopen(Paths::configPath().c_str(), WRITE_ACCES_SPECIFIER);
    if (!config)
    {
        LOG("Opening config file at %s failed", Paths::configPath().c_str());
        return;
    }
    char writeBuffer[65536];
    FileWriteStream os(config, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer(os);
    defa->Accept(writer);
    fclose(config);
}

void Conf::setFromFile(rapidjson::Document *params)
{
    boardWidth = (*params)[keys[0]].GetInt();
    boardHeight = (*params)[keys[1]].GetInt();
    startInterval = (*params)[keys[2]].GetInt();
    screenWidth = (*params)[keys[3]].GetInt();
    screenHeight = (*params)[keys[4]].GetInt();
    slideSpeed = (*params)[keys[5]].GetInt();
}

} /* namespace tet */

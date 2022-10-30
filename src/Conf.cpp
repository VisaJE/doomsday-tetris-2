/*
 * Conf.cpp
 *
 *  Created on: 31.5.2018
 *      Author: Eemil
 */

#include "Conf.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include "Paths.h"

using namespace rapidjson;

#ifdef WIN32
#define READ_ACCES_SPECIFIER "rb"
#define WRITE_ACCES_SPECIFIER "rb"
#else
#define READ_ACCES_SPECIFIER "r"
#define WRITE_ACCES_SPECIFIER "r"
#endif

namespace tet {
// Default values are set at the constructor.
Conf::Conf() {
    const char norm[] = " { \"BOARD_WIDTH\": 10 , \"BOARD_HEIGHT\": 19, \"START_INTERVAL\": 500, \"SCREEN_WIDTH\": 600, \"SCREEN_HEIGHT\": 900, \"SLIDE_SPEED\": 5 } ";
    Document defa;
    defa.Parse(norm);
    FILE* config = fopen(Paths::configPath().c_str(), READ_ACCES_SPECIFIER);
    if (!config) {
        LOG("No config\n");
        fallBack(&config, &defa);
        setFromFile(&defa);
    }
    else {
        char readBuffer[65536];
        FileReadStream in(config, readBuffer, sizeof(readBuffer));
        Document found;
        found.ParseStream(in);
        if (!checkValidity(&found)) {
            LOG("Non valid config");
            fclose(config);
            fallBack(&config, &defa);
            setFromFile(&defa);
        }
        else {
            LOG("Config read");
            setFromFile(&found);
            fclose(config);
        }
    }

}

Conf::~Conf() {
}


bool Conf::competitionValid() {
    bool valid = true;
    if (startInterval > 500) valid = false;
    if (slideSpeed > 8) valid = false;
    if (!(boardWidth == 10 && boardHeight == 19)) valid = false;
    return valid;
}


bool Conf::checkValidity(Document *d) {
    bool valid = true;
     if (!d->IsObject()) valid = false;
     // Lets check the keys with int values.
     for (int i = 0; i < 6; i++) {
         Value::MemberIterator here = d->FindMember(keys[i]);
         if (here == d->MemberEnd()) valid = false;
         if (!(here->value.IsNumber()) || here->value.GetInt() <= 0) valid = false;
     }
     return valid;
}


void Conf::fallBack(FILE* *config, rapidjson::Document *defa) {
    *config = fopen(Paths::configPath().c_str(), WRITE_ACCES_SPECIFIER);
    char writeBuffer[65536];
    FileWriteStream os(*config, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer(os);
    defa->Accept(writer);
    fclose(*config);
}


void Conf::setFromFile(rapidjson::Document *params) {
    boardWidth = (*params)[keys[0]].GetInt();
    boardHeight = (*params)[keys[1]].GetInt();
    startInterval = (*params)[keys[2]].GetInt();
    screenWidth = (*params)[keys[3]].GetInt();
    screenHeight = (*params)[keys[4]].GetInt();
    slideSpeed = (*params)[keys[5]].GetInt();
}


} /* namespace tet */

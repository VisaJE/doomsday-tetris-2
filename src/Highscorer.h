/*
 * Highscorer.h
 *
 *  Created on: 6.6.2018
 *      Author: Eemil
 */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include "UniqueIdentifier.h"
#ifndef SRC_HIGHSCORER_H_
#define SRC_HIGHSCORER_H_
#include <string>

namespace tet {

class Highscorer {
public:
    Highscorer(const char* hs_filename);
    virtual ~Highscorer();
    void getHighscore(std::string name[10], int score[10]);
    void getHighscore(std::string name[10], int score[10], Uid id[10]);

    bool addScore(std::string name, int score);
    void replaceList(std::string names[10], int scores[10], Uid ids[10]);
    int getHighest();
    int getLowest();
private:
    const char* hsFilename;
    FILE* file;
    void setFile(std::string t);
    std::string readFile();
    bool validate(rapidjson::Document *d);
    bool fileExists(const std::string& filename);
    void updateLists(rapidjson::Document *d);
    void refreshFile();
    void sort(std::string name[10], int score[10], Uid ids[10]);
    void sort(std::string name[10], int score[10]);
    std::string currentBoard[10];
    int currentScore[10];
    Uid currentId[10];
    const std::string key = "1";
    const std::string prototype = "{\"name\" : [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"], \"score\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0], \"uid\":[\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"]}";
};

} /* namespace tet */

#endif /* SRC_HIGHSCORER_H_ */

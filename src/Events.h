#pragma once
/*
 * Events.h
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#include "Screen.h"
#include "Grid.h"
#include "UberBlockifier.h"
#include "Highscorer.h"
#include "GlobalHighscore.h"
#include "queue"
#include "ProcessTimer.h"

namespace tet {

class Events {
public:
    Events(Screen *screen, Grid *g, const int startInterval, int slideSpeed, bool scoreable, bool &fastDropInitiated);
    virtual ~Events();
    bool paused;
    bool quit;
    int slideSpeed;
    bool goingLeft();
    bool goingRight();
    Grid *g;
    int init();
    int menu();
    int globalScoreList();
    Uint32 currentInterval;
    SDL_mutex *mutex;
    std::queue<Grid::GridFunc> callQue;
    Highscorer& getLocalHighscores();
    Highscorer hs;
private:
    GlobalHighscore globalHs;
    bool fairToScore;
    int setHighscore();
    int startInt;
    int baseInterval;
    bool speedUpdated =true;//Keeps track whether the base interval has been updated for this block.
    SDL_Event event;
    Screen* screen;
    bool& sPressed; //sPressed is now a reference to a common bool between events and grid.
    bool aPressed = false;
    bool dPressed = false;
    bool pause();
    void setDropSpeed();
};

} /* namespace tet */


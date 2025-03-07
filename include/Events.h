#pragma once
/*
 * Events.h
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include "GlobalHighscore.h"
#include "Grid.h"
#include "Highscorer.h"
#include "Screen.h"
#include "UberBlockifier.h"
#include "queue"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_stdinc.h>
#include <atomic>
#include <cstring>
#include <iostream>

namespace tet
{

class Events
{
  public:
    Events(Screen *screen, Grid *g, const int startInterval, int slideSpeed, bool scoreable,
           bool &fastDropInitiated);
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
    Highscorer &getLocalHighscores();
    Highscorer hs;

  private:
    GlobalHighscore globalHs;
    bool fairToScore;
    int setHighscore();
    int startInt;
    int baseInterval;
    bool speedUpdated =
        true; // Keeps track whether the base interval has been updated for this block.
    SDL_Event event;
    Screen *screen;
    bool &sPressed; // sPressed is now a reference to a common bool between events and grid.
    bool aPressed = false;
    bool dPressed = false;
    bool pause();
    void setDropSpeed();

    // Screen updating
    SDL_mutex *screenMutex;
    std::function<void()> screenUpdate;
    std::atomic<bool> invalidated;
    SDL_TimerID updateTimer;
    static Uint32 updateTask(Uint32 interval, void *arg);

    template <typename C> void updateScreen(const C &lambda)
    {
        if (!invalidated) // Lock is not needed if screenUpdate is not accessed.
        {
            if (SDL_LockMutex(screenMutex) == 0)
            {
                screenUpdate = lambda;
                SDL_UnlockMutex(screenMutex);
            }
        }
        else
            screenUpdate = lambda;
        invalidated = true;
    }
};

} /* namespace tet */

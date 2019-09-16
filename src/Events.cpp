/*
 * Events.cpp
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#include "Screen.h"
#include "Events.h"
#include "Grid.h"
#include "UberBlockifier.h"
#include <thread>
#include "math.h"
#include "Log.h"
#include "Paths.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))()

namespace tet {


Events::Events(Screen *s, Grid *g, int startInterval, int slideSpeed, bool scoreable, bool& fastDropInitiated):
    paused(true),
    slideSpeed(slideSpeed),
    g(g),
    currentInterval(startInterval),
    hs(Highscorer(Paths::highscorePath().c_str())),
    globalHs(GlobalHighscore(&hs)),
    fairToScore(scoreable),
    startInt(startInterval),
    baseInterval(startInt),
    screen(s),
    sPressed(fastDropInitiated)
{
    quit = false;
    mutex = SDL_CreateMutex();
}

Events::~Events() {
    SDL_UnlockMutex(mutex);
    SDL_DestroyMutex(mutex);
}


bool Events::goingLeft() {
    return aPressed;
}


bool Events::goingRight() {
    return dPressed;
}

Uint32 voidFunc(Uint32 interval, void *arg) {
    (void)interval;
    (void)arg;
    return interval;
}
Uint32 ticker(Uint32 interval, void * arg) {
    (void)interval;
    Events *event = (Events*)arg;
    if (SDL_LockMutex(event->mutex)==0) {
        if (event->g->lost || event->paused || event->quit) {
            SDL_UnlockMutex(event->mutex);
            return 0;
        }
        event->callQue.push(&Grid::wholeTick);
        SDL_UnlockMutex(event->mutex);
    }
    return event->currentInterval;
}


Uint32 sliderL(Uint32 interval, void * arg) {
    (void)interval;
    Events *event = (Events*)arg;
    if (SDL_LockMutex(event->mutex) == 0) {
        if (event->g->lost || event->paused || event->quit) {
            SDL_UnlockMutex(event->mutex);
            return 0;
        }
        if (event->goingLeft()) event->callQue.push(&Grid::moveL);
        SDL_UnlockMutex(event->mutex);
    }
    return max(event->currentInterval*(1.0/event->slideSpeed), 60.0);
}


Uint32 sliderR(Uint32 interval, void * arg) {
    (void)interval;
    Events *event = (Events*)arg;
    if (SDL_LockMutex(event->mutex) == 0) {
        if (event->g->lost || event->paused || event->quit) {
            SDL_UnlockMutex(event->mutex);
            return 0;
        }
        if (event->goingRight()) event->callQue.push(&Grid::moveR);
        SDL_UnlockMutex(event->mutex);
    }
    return max(event->currentInterval*(1.0/event->slideSpeed), 60.0);
}


void Events::setDropSpeed() {
    if (g->droppedAmount() % 10 == 0) {
        if (!speedUpdated) {
            baseInterval = max((int)(startInt - sqrt((g->droppedAmount()/10)*10000)), 10);
            speedUpdated=true;
        }
    } else speedUpdated=false;
    if (!sPressed) currentInterval = baseInterval;
    else currentInterval =baseInterval/6;
    //cout << "Current interval set to " << currentInterval << endl << "Is s pressed " << sPressed << endl;
}


int Events::setHighscore() {
    bool done = false;
    if (!fairToScore) {
        screen->gameOver("Your configuration is not fair for scoring.");
        while(!quit && !done && SDL_WaitEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                quit = true;
                break;

                case SDL_KEYDOWN:
                done = true;
            }
        }
        return 0;
    }
    if (g->getPoints()<hs.getLowest())
    {

        screen->gameOver("Your score was not enough\nto reach the leaderboard.");
        while(!quit && !done && SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                quit = true;
                break;

                case SDL_KEYDOWN:
                done = true;

            }
        }
        return 0;
    }
    string t;
    short len = 0;
    string text ="Enter your name to be\nremembered among the best:\n";
    screen->gameOver(text);
    SDL_StartTextInput();
            while(!quit && !done && SDL_WaitEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                    quit = true;
                    break;

                    case SDL_TEXTINPUT:
                    /* Add new text onto the end of our text */
                    if (len < 10) {
                        t.append(event.text.text);
                        ++len;
                        string temp = text;
                        screen->gameOver(temp.append(t));
                    } // @suppress("No break at end of case")
                    case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_BACKSPACE:
                        if (len > 0) {
                            --len;
                            if((int) t.back()< 0) t.pop_back();
                            t.pop_back();
                            string temp = text;
                            screen->gameOver(temp.append(t));
                        }
                        break;
                        case SDLK_RETURN :
                        if (len > 0) {
                            while (t.length()> 10) {
                                if((int) t.back()< 0) t.pop_back();
                                t.pop_back();
                            }
                            hs.addScore(t, g->getPoints());
                            done = true;
                        }
                        else {
                            text = "Enter even something to be\nremembered among the best:\n";
                            screen->gameOver(text);
                        }
                        break;
                    }
                    break;
                }
            }
    SDL_StopTextInput();
    return 0;
}


Highscorer& Events::getLocalHighscores()
{
    return hs;
}

int Events::menu() {
    string names[10];
    int scores[10];
    hs.getHighscore(names, scores);
    screen->menu(names, scores);
    int err = 0;

    bool sizeCh = false;
    int width = 0;
    int height = 0;

    while(!quit && SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            quit = true;
            break;

            case SDL_MOUSEBUTTONDOWN:
            break;

            case SDL_MOUSEBUTTONUP:
            break;

            case SDL_WINDOWEVENT :
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    LOG("Resize event\n");
                    if (event.window.windowID == screen->windowID)
                    {
                        width = event.window.data1;
                        height = event.window.data2;
                        LOG("Changed window size to %d, %d\n", width, height);
                        sizeCh = true;
                    }
                    break;
                }
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    break;
                }
                default:
                {
                    LOG("Refreshing\n");
                    screen->menu(names, scores);
                }
            }
            break;

            case SDL_KEYDOWN :
            switch (event.key.keysym.sym)
            {
                case SDLK_RETURN :
                err = init();
                if (g->lost && err == 0)
                {
                    err = setHighscore();
                    g->reset();
                    while (callQue.size()!=0)
                    {
                      callQue.pop();
                    }
                }
                hs.getHighscore(names, scores);
                screen->menu(names, scores);
                break;

                case SDLK_g :
                globalScoreList();
                screen->menu(names, scores);
                break;

                case SDLK_q:
                quit = true;
                break;
            }
            break;
        }
        if (sizeCh)
        {
            screen->changeSize(height, width);
            screen->menu(names, scores);
            sizeCh = false;
        }
    }
    return err;
}


int Events::init() {
    speedUpdated = false;
    setDropSpeed();
    paused = false;
    SDL_TimerID timer = SDL_AddTimer(startInt, &ticker, this);
    SDL_TimerID slideRTimer = SDL_AddTimer(startInt, &voidFunc, this);
    SDL_TimerID slideLTimer = SDL_AddTimer(startInt, &voidFunc, this);
    //This is done to remove a warning of non initialized timers.
    SDL_RemoveTimer(slideLTimer);
    SDL_RemoveTimer(slideRTimer);
    aPressed = false;
    sPressed = false;
    dPressed = false;
    screen->printGrid();
    while (!quit) {
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                std::cout << "Quitting..." << std::flush;
                SDL_UnlockMutex(mutex);
                quit = true;
                SDL_RemoveTimer(timer);
                if (aPressed) {SDL_RemoveTimer(slideLTimer);}
                if (dPressed) {SDL_RemoveTimer(slideRTimer);}
                break;

                case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                // MOVE LEFT
                    case SDLK_a: case SDLK_LEFT:
                    if (!aPressed)
                    {
                        aPressed = true;
                        if (SDL_LockMutex(mutex)==0) {
                            callQue.push(&Grid::moveL);
                            SDL_UnlockMutex(mutex);
                        }
                        slideLTimer = SDL_AddTimer((Uint32)max((int)(currentInterval*(1.0/slideSpeed)+40), 60), &sliderL, this);
                    }
                    break;

                    // MOVE RIGHT
                    case SDLK_d: case SDLK_RIGHT:
                    if (!dPressed)
                    {
                        dPressed = true;
                        if (SDL_LockMutex(mutex)==0)
                        {
                            callQue.push(&Grid::moveR);
                            SDL_UnlockMutex(mutex);
                        }
                        slideRTimer = SDL_AddTimer((Uint32)max((int)(currentInterval*(1.0/slideSpeed)+40), 60), &sliderR, this);
                    }
                    break;

                    // INSTANT DROP
                    case SDLK_SPACE:
                    if (SDL_LockMutex(mutex) == 0)
                    {
                        this->callQue.push(&Grid::wholeDrop);
                        SDL_UnlockMutex(mutex);
                    }
                    break;

                    // FAST DROP
                    case SDLK_s : case SDLK_DOWN :
                    if (!sPressed)
                    {
                        if (SDL_LockMutex(mutex) == 0)
                        {
                            this->callQue.push(&Grid::wholeTick);
                            SDL_UnlockMutex(mutex);
                        }
                        SDL_RemoveTimer(timer);
                        sPressed = true;
                        setDropSpeed();
                        timer =  SDL_AddTimer(currentInterval, &ticker, this);

                    }
                    break;

                    // ROTATE
                    case SDLK_w : case SDLK_UP :
                    if (SDL_LockMutex(mutex)==0) {
                        this->callQue.push(&Grid::rotate);
                        SDL_UnlockMutex(mutex);
                    }
                    break;

                    // PAUSE
                    case SDLK_p :
                    paused = true;
                    SDL_RemoveTimer(timer);
                    if (aPressed)
                    {
                        SDL_RemoveTimer(slideLTimer);
                        aPressed = false;
                    }
                    if (dPressed) {SDL_RemoveTimer(slideRTimer);
                        dPressed=false;
                    }
                    sPressed = false;
                    if (pause())
                    {
                        timer = SDL_AddTimer(currentInterval, &ticker, this);
                        g->wholeTick();
                        screen->printGrid();
                    }
                    break;

                    // RESTART
                    case SDLK_r :
                    try
                    {
                        SDL_RemoveTimer(timer);
                        if (aPressed)
                        {
                            SDL_RemoveTimer(slideLTimer);
                            aPressed = false;
                        }
                        if (dPressed)
                        {
                            SDL_RemoveTimer(slideRTimer);
                            dPressed = false;
                        }
                        SDL_LockMutex(mutex);
                        while (!callQue.empty()) {callQue.pop();}
                        SDL_UnlockMutex(mutex);
                        g->reset();
                        timer = SDL_AddTimer(startInt, &ticker, this);
                        screen->printGrid();
                    }
                    catch (int i) {}
                    speedUpdated = false;
                    setDropSpeed();
                    break;

                    // If game over go to menu.
                    case SDLK_RETURN :
                    if (g->lost)
                    {
                        SDL_UnlockMutex(mutex);
                        SDL_RemoveTimer(timer);
                        if (aPressed)
                        {
                            SDL_RemoveTimer(slideLTimer);
                            aPressed=false;
                        }
                        if (dPressed)
                        {
                            SDL_RemoveTimer(slideRTimer);
                            dPressed=false;
                        }
                        return 0;
                    }
                    else break;

                    // MENU
                    case SDLK_ESCAPE :
                    SDL_UnlockMutex(mutex);
                    SDL_RemoveTimer(timer);
                    g->wholeTick();
                    if (aPressed)
                    {
                        SDL_RemoveTimer(slideLTimer);
                        aPressed=false;
                    }
                    if (dPressed)
                    {
                        SDL_RemoveTimer(slideRTimer);
                        dPressed=false;
                    }
                    sPressed = false;
                    return 0;

#ifdef DEBUG
                    // ERROR INFO
                    case SDLK_0 :
                    cout << "Debug info:" << endl << "Buttons pressed (a,s,d): " << aPressed << sPressed << dPressed << endl;
                    cout << "Current interval: " << currentInterval << " Stack size: " << callQue.size() << endl;
                    cout << "Game status (lost, paused, quit)" << g->lost << paused << quit << endl;
                    cout << "Points:" << g->getPoints() << " Dropped blocks: " << g->droppedAmount() << endl;
#endif

                }
                break;

                case SDL_KEYUP :
                switch (event.key.keysym.sym)
                {
                    case SDLK_a: case SDLK_LEFT:
                    if (aPressed)
                    {
                        aPressed = false;
                        SDL_RemoveTimer(slideLTimer);
                    }
                    break;

                    case SDLK_d: case SDLK_RIGHT:
                    if (dPressed)
                    {
                        dPressed = false;
                        SDL_RemoveTimer(slideRTimer);
                    }
                    break;

                    case SDLK_s : case SDLK_DOWN :
                    if (sPressed)
                    {
                        SDL_RemoveTimer(timer);
                        sPressed = false;
                        setDropSpeed();
                        timer =  SDL_AddTimer(currentInterval, &ticker, this);
                    }
                    break;

                }
                break;

                default :
                {
                }
            }

        }
        // Update the drop speed.
        setDropSpeed();

        // Do the next thing on call queue.
        try
        {
            if (SDL_LockMutex(mutex)==0)
            {
                if (!callQue.empty() && !g->lost)
                {
                    Grid::GridFunc func = callQue.front();
                    CALL_MEMBER_FN(*g, func);
                    callQue.pop();
            //                g.printGrid();
                    screen->printGrid();

                }
                SDL_UnlockMutex(mutex);
            }
        }
        catch (int e)
        {
            SDL_UnlockMutex(mutex);
        }
    }
    return 0;
}


bool Events::pause()
{
    paused = true;
    screen->pause();
        while(!quit && paused && SDL_WaitEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                quit = true;
                break;

                case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_p:
                    paused = false;
                    break;
                }

                case SDL_WINDOWEVENT:
                LOG("Refreshing");
                screen->pause();
                break;
            }
        }
    setDropSpeed();
    return !quit;
}

int Events::globalScoreList()
{
    globalHs.updateData();
    screen->gScorePanel(globalHs.names, globalHs.scores);
    int err = 0;
        while(!quit && SDL_WaitEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                quit = true;
                break;

                case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_RETURN:
                    return err;
                    break;

                    case SDLK_ESCAPE:
                    return err;
                    break;
                }
                break;

                case SDL_WINDOWEVENT:
                LOG("Refreshing\n");
                screen->gScorePanel(globalHs.names, globalHs.scores);
                break;
            }
        }
    return err;
}

} /* namespace tet */

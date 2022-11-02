/*
 * Events.cpp
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include "Events.h"
#include "Block.h"
#include "Grid.h"
#include "Log.h"
#include "Paths.h"
#include "Screen.h"
#include "UberBlockifier.h"
#include "math.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_stdinc.h>
#include <cstring>
#include <iostream>
#include <thread>

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))()

namespace tet
{

Uint32 Events::updateTask(Uint32 interval, void *arg)
{
    Events *event = reinterpret_cast<Events *>(arg);
    if (event->invalidated)
    {
        if (!SDL_LockMutex(event->screenMutex))
        {
            if (event->screenUpdate)
                event->screenUpdate();
            SDL_UnlockMutex(event->screenMutex);
        }
        event->invalidated = false;
    }
    return interval;
}

Events::Events(Screen *s, Grid *g, int startInterval, int slideSpeed, bool scoreable,
               bool &fastDropInitiated)
    : paused(true), slideSpeed(slideSpeed), g(g), currentInterval(startInterval),
      hs(Highscorer(Paths::highscorePath().c_str())), globalHs(GlobalHighscore(&hs)),
      fairToScore(scoreable), startInt(startInterval), baseInterval(startInt), screen(s),
      sPressed(fastDropInitiated)
{
    quit = false;
    mutex = SDL_CreateMutex();
    screenMutex = SDL_CreateMutex();
    updateTimer = SDL_AddTimer(16, &updateTask, this);
    g->onNewBlock = [this](Block &block) {
        SDL_LockMutex(screenMutex);
        this->screen->showBlock(block);
        SDL_UnlockMutex(screenMutex);
    };
}

Events::~Events()
{
    SDL_RemoveTimer(updateTimer);
    SDL_UnlockMutex(mutex);
    SDL_DestroyMutex(mutex);
    SDL_UnlockMutex(screenMutex);
    SDL_DestroyMutex(screenMutex);
}

bool Events::goingLeft() { return aPressed; }

bool Events::goingRight() { return dPressed; }

Uint32 voidFunc(Uint32 interval, void *arg)
{
    (void)interval;
    (void)arg;
    return interval;
}
Uint32 ticker(Uint32 interval, void *arg)
{
    (void)interval;
    Events *event = (Events *)arg;
    if (SDL_LockMutex(event->mutex) == 0)
    {
        if (event->g->lost || event->paused || event->quit)
        {
            SDL_UnlockMutex(event->mutex);
            return 0;
        }
        event->callQue.push(&Grid::wholeTick);
        SDL_UnlockMutex(event->mutex);
    }
    return event->currentInterval;
}

Uint32 sliderL(Uint32 interval, void *arg)
{
    (void)interval;
    Events *event = (Events *)arg;
    if (SDL_LockMutex(event->mutex) == 0)
    {
        if (event->g->lost || event->paused || event->quit)
        {
            SDL_UnlockMutex(event->mutex);
            return 0;
        }
        if (event->goingLeft())
            event->callQue.push(&Grid::moveL);
        SDL_UnlockMutex(event->mutex);
    }
    return max(event->currentInterval * (1.0 / event->slideSpeed), 60.0);
}

Uint32 sliderR(Uint32 interval, void *arg)
{
    (void)interval;
    Events *event = (Events *)arg;
    if (SDL_LockMutex(event->mutex) == 0)
    {
        if (event->g->lost || event->paused || event->quit)
        {
            SDL_UnlockMutex(event->mutex);
            return 0;
        }
        if (event->goingRight())
            event->callQue.push(&Grid::moveR);
        SDL_UnlockMutex(event->mutex);
    }
    return max(event->currentInterval * (1.0 / event->slideSpeed), 60.0);
}

void Events::setDropSpeed()
{
    if (g->droppedAmount() % 10 == 0)
    {
        if (!speedUpdated)
        {
            baseInterval = max((int)(startInt - sqrt((g->droppedAmount() / 10) * 10000)), 10);
            speedUpdated = true;
        }
    }
    else
        speedUpdated = false;
    if (!sPressed)
        currentInterval = baseInterval;
    else
        currentInterval = baseInterval / 6;
    // cout << "Current interval set to " << currentInterval << endl << "Is s pressed " << sPressed
    // << endl;
}

int Events::setHighscore()
{
    bool done = false;
    if (!fairToScore)
    {
        updateScreen(
            [this]() { this->screen->gameOver("Your configuration is not fair for scoring."); });
        while (!quit && !done && SDL_WaitEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT: quit = true; break;

            case SDL_KEYDOWN: done = true;
            }
        }
        return 0;
    }
    if (g->getPoints() < hs.getLowest())
    {

        updateScreen(
            [this]() { screen->gameOver("Your score was not enough\nto reach the leaderboard."); });
        while (!quit && !done && SDL_WaitEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT: quit = true; break;

            case SDL_KEYDOWN: done = true;
            }
        }
        return 0;
    }
    string t;
    short len = 0;
    string text = "Enter your name to be\nremembered among the best:\n";
    updateScreen([this, text]() { screen->gameOver(text); });
    SDL_StartTextInput();
    while (!quit && !done && SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: quit = true; break;

        case SDL_TEXTINPUT:
            /* Add new text onto the end of our text */
            if (len < 10)
            {
                t.append(event.text.text);
                ++len;
                updateScreen([this, text, t]() { screen->gameOver(text + t); });
            } // @suppress("No break at end of case")
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_BACKSPACE:
                if (len > 0)
                {
                    --len;
                    if ((int)t.back() < 0)
                        t.pop_back();
                    t.pop_back();
                    updateScreen([this, text, t]() { screen->gameOver(text + t); });
                }
                break;
            case SDLK_RETURN:
                if (len > 0)
                {
                    while (t.length() > 10)
                    {
                        if ((int)t.back() < 0)
                            t.pop_back();
                        t.pop_back();
                    }
                    hs.addScore(t, g->getPoints());
                    done = true;
                }
                else
                {
                    text = "Enter even something to be\nremembered among the best:\n";
                    updateScreen([this, text, t]() { screen->gameOver(text); });
                }
                break;
            }
            break;
        }
    }
    SDL_StopTextInput();
    return 0;
}

Highscorer &Events::getLocalHighscores() { return hs; }

int Events::menu()
{
    string names[10];
    int scores[10];
    hs.getHighscore(names, scores);

    auto showMenu = [this, &names, &scores]() {
        if (SDL_LockMutex(screenMutex) == 0)
        {
            screen->menu(names, scores);
            SDL_UnlockMutex(screenMutex);
        }
    };
    showMenu();

    int err = 0;

    int height = 0;
    int width = 0;

    while (!quit)
    {
        auto happening = SDL_WaitEventTimeout(&event, 200);

        if (bool(happening))
        {
            switch (event.type)
            {
            case SDL_QUIT: quit = true; break;

            case SDL_MOUSEBUTTONDOWN: break;
            case SDL_MOUSEBUTTONUP: break;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    LOG("Resize event\n");
                    if (event.window.windowID == screen->windowID)
                    {
                        width = event.window.data1;
                        height = event.window.data2;
                        if (SDL_LockMutex(screenMutex) == 0)
                        {
                            screen->changeSize(height, width);
                            SDL_UnlockMutex(screenMutex);
                        }
                    }
                    break;

                case SDL_WINDOWEVENT_SIZE_CHANGED: break;

                default: LOG("Refreshing\n"); showMenu();
                }
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_RETURN:
                    err = init();
                    if (g->lost && err == 0)
                    {
                        err = setHighscore();
                        if (SDL_LockMutex(screenMutex))
                            throw 2953;
                        g->reset();
                        while (callQue.size() != 0)
                        {
                            callQue.pop();
                        }
                    }
                    hs.getHighscore(names, scores);
                    showMenu();
                    break;

                case SDLK_g:
                    globalScoreList();
                    showMenu();
                    break;

                case SDLK_q:
                    LOG("Quit called");
                    quit = true;
                    break;
                }
                break;
            }
        }
        else
        {
            showMenu();
        }
    }
    return err;
}

int Events::init()
{
    if (SDL_LockMutex(screenMutex))
        throw 420;
    g->reset();
    SDL_UnlockMutex(screenMutex);
    speedUpdated = false;
    setDropSpeed();
    paused = false;
    SDL_TimerID timer = SDL_AddTimer(startInt, &ticker, this);
    SDL_TimerID slideRTimer = SDL_AddTimer(startInt, &voidFunc, this);
    SDL_TimerID slideLTimer = SDL_AddTimer(startInt, &voidFunc, this);
    // This is done to remove a warning of non initialized timers.
    SDL_RemoveTimer(slideLTimer);
    SDL_RemoveTimer(slideRTimer);
    aPressed = false;
    sPressed = false;
    dPressed = false;
    updateScreen([this]() { this->screen->printGrid(); });
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                std::cout << "Quitting..." << std::flush;
                SDL_UnlockMutex(mutex);
                quit = true;
                SDL_RemoveTimer(timer);
                if (aPressed)
                    SDL_RemoveTimer(slideLTimer);
                if (dPressed)
                    SDL_RemoveTimer(slideRTimer);
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    // MOVE LEFT
                case SDLK_a:
                case SDLK_LEFT:
                case SDLK_j:
                    if (!aPressed)
                    {
                        aPressed = true;
                        if (SDL_LockMutex(mutex) == 0)
                        {
                            callQue.push(&Grid::moveL);
                            SDL_UnlockMutex(mutex);
                        }
                        slideLTimer = SDL_AddTimer(
                            (Uint32)max((int)(currentInterval * (1.0 / slideSpeed) + 40), 60),
                            &sliderL, this);
                    }
                    break;

                // MOVE RIGHT
                case SDLK_d:
                case SDLK_RIGHT:
                case SDLK_l:
                    if (!dPressed)
                    {
                        dPressed = true;
                        if (SDL_LockMutex(mutex) == 0)
                        {
                            callQue.push(&Grid::moveR);
                            SDL_UnlockMutex(mutex);
                        }
                        slideRTimer = SDL_AddTimer(
                            (Uint32)max((int)(currentInterval * (1.0 / slideSpeed) + 40), 60),
                            &sliderR, this);
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
                case SDLK_s:
                case SDLK_DOWN:
                case SDLK_k:
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
                        timer = SDL_AddTimer(currentInterval, &ticker, this);
                    }
                    break;

                // ROTATE
                case SDLK_w:
                case SDLK_UP:
                case SDLK_i:
                    if (SDL_LockMutex(mutex) == 0)
                    {
                        this->callQue.push(&Grid::rotate);
                        SDL_UnlockMutex(mutex);
                    }
                    break;

                // PAUSE
                case SDLK_p:
                pauseGame:
                    paused = true;
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
                    sPressed = false;
                    if (pause())
                    {
                        timer = SDL_AddTimer(currentInterval, &ticker, this);
                        if (SDL_LockMutex(screenMutex))
                            throw 23955;
                        this->g->wholeTick();
                        SDL_UnlockMutex(screenMutex);
                        updateScreen([this]() { this->screen->printGrid(); });
                    }
                    break;

                // RESTART
                case SDLK_r:
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
                        if (SDL_LockMutex(mutex))
                            throw 241;
                        while (!callQue.empty())
                        {
                            callQue.pop();
                        }
                        SDL_UnlockMutex(mutex);
                        if (SDL_LockMutex(screenMutex))
                            throw 242;
                        g->reset();
                        SDL_UnlockMutex(screenMutex);
                        timer = SDL_AddTimer(startInt, &ticker, this);
                        updateScreen([this]() { screen->printGrid(); });
                    }
                    catch (int i)
                    {
                    }
                    speedUpdated = false;
                    setDropSpeed();
                    break;

                // If game over go to menu.
                case SDLK_RETURN:
                    if (g->lost)
                    {
                        SDL_UnlockMutex(mutex);
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
                        return 0;
                    }
                    else
                        break;

                // MENU
                case SDLK_ESCAPE: goto returnToMenu;

#ifdef DEBUG
                // ERROR INFO
                case SDLK_0:
                    cout << "Debug info:" << endl
                         << "Buttons pressed (a,s,d): " << aPressed << sPressed << dPressed << endl;
                    cout << "Current interval: " << currentInterval
                         << " Stack size: " << callQue.size() << endl;
                    cout << "Game status (lost, paused, quit)" << g->lost << paused << quit << endl;
                    cout << "Points:" << g->getPoints() << " Dropped blocks: " << g->droppedAmount()
                         << endl;
#endif
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_a:
                case SDLK_LEFT:
                case SDLK_j:
                    if (aPressed)
                    {
                        aPressed = false;
                        SDL_RemoveTimer(slideLTimer);
                    }
                    break;

                case SDLK_d:
                case SDLK_RIGHT:
                case SDLK_l:
                    if (dPressed)
                    {
                        dPressed = false;
                        SDL_RemoveTimer(slideRTimer);
                    }
                    break;

                case SDLK_s:
                case SDLK_DOWN:
                case SDLK_k:
                    if (sPressed)
                    {
                        SDL_RemoveTimer(timer);
                        sPressed = false;
                        setDropSpeed();
                        timer = SDL_AddTimer(currentInterval, &ticker, this);
                    }
                    break;
                }
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    // goto returnToMenu;
                    goto pauseGame;

                default: {
                }
                }
                break;

            default: {
            }
            }
        }
        // Update the drop speed.
        setDropSpeed();

        // Do the next thing on call queue.
        try
        {
            if (SDL_LockMutex(mutex) == 0)
            {
                if (!callQue.empty() && !g->lost)
                {
                    Grid::GridFunc func = callQue.front();
                    if (SDL_LockMutex(screenMutex))
                        throw 2452;
                    CALL_MEMBER_FN(*g, func);
                    SDL_UnlockMutex(screenMutex);
                    callQue.pop();
                    //                g.printGrid();
                    updateScreen([this]() { screen->printGrid(); });
                }
                SDL_UnlockMutex(mutex);
            }
        }
        catch (int e)
        {
            SDL_UnlockMutex(mutex);
            SDL_UnlockMutex(screenMutex);
        }
    }
returnToMenu:
    SDL_UnlockMutex(mutex);
    SDL_UnlockMutex(screenMutex);
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
    sPressed = false;
    return 0;
}

bool Events::pause()
{
    paused = true;
    updateScreen([this]() { screen->pause(); });
    while (!quit && paused && SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: quit = true; break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_p: paused = false; break;
            }

        case SDL_WINDOWEVENT:
            LOG("Refreshing");
            updateScreen([this]() { screen->pause(); });
            break;
        }
    }
    setDropSpeed();
    return !quit;
}

int Events::globalScoreList()
{
    globalHs.updateData();
    updateScreen([this]() { screen->gScorePanel(globalHs.names, globalHs.scores); });
    int err = 0;
    while (!quit && SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: quit = true; break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_RETURN: return err; break;

            case SDLK_ESCAPE: return err; break;
            }
            break;

        case SDL_WINDOWEVENT:
            LOG("Refreshing\n");
            updateScreen([this]() { screen->gScorePanel(globalHs.names, globalHs.scores); });
            break;
        }
    }
    return err;
}

} /* namespace tet */

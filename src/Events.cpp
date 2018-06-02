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

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))()

namespace tet {

// Call structures
struct Tick {
	void operator()() const
	{
    	std::cout << "Hello, World!" << std::endl;
	}
};

Events::Events(Screen s, Grid g): paused(true),  g(g) {
	screen = s;
	quit = false;
}

Events::~Events() {
}


Uint32 ticker(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (event->g.lost || event->paused || event->quit) {
		return 0;
	}
	event->callQue.push(&Grid::wholeTick);
	cout << event->callQue.size();
	return event->currentInterval;
}


Uint32 sliderL(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (event->g.lost || event->paused || event->quit) {
		return 0;
	}
	event->callQue.push(&Grid::moveL);
	return (Uint32)event->currentInterval*0.2;
}


Uint32 sliderR(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (event->g.lost || event->paused || event->quit) {
		return 0;
	}
	event->callQue.push(&Grid::moveR);
	return (Uint32)event->currentInterval*0.2;
}


void Events::setDropSpeed() {
	currentInterval = 1000;
}


void Events::init() {
	paused = false;
	SDL_TimerID timer = SDL_AddTimer(currentInterval, &ticker, this);
	SDL_TimerID slideRTimer;
	SDL_TimerID slideLTimer;
	while (!quit || aPressed || dPressed) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					std::cout << "Quit order..." << std::flush;
					paused = true;
					quit = true;
					SDL_RemoveTimer(timer);
					if (aPressed) {SDL_RemoveTimer(slideLTimer);}
					if (dPressed) {SDL_RemoveTimer(slideRTimer);}
					break;
				}
				case SDL_KEYDOWN:  {
					cout << "Key pressed." << flush;
					switch (event.key.keysym.sym) {
						case SDLK_a: case SDLK_LEFT: {
							if (!aPressed) {
								aPressed = true;
								callQue.push(&Grid::moveL);
								slideLTimer = SDL_AddTimer(currentInterval*0.16, &sliderL, this);
							}
							break;
						}
						case SDLK_d: case SDLK_RIGHT: {
							if (!dPressed) {
								dPressed = true;
								callQue.push(&Grid::moveR);
								slideRTimer = SDL_AddTimer(currentInterval*0.16, &sliderR, this);
							}
							break;
						}
						case SDLK_SPACE: {
							this->callQue.push(&Grid::wholeDrop);
							break;
						}
						case SDLK_s : case SDLK_DOWN : {
							if (!sPressed) {
								this->callQue.push(&Grid::wholeTick);
								currentInterval /= 2;
							}
							break;
						}
						case SDLK_w : case SDLK_UP : {
							this->callQue.push(&Grid::rotate);
							break;
						}
					}
					break;
				}
				case SDL_KEYUP : {
					switch (event.key.keysym.sym) {
						case SDLK_a: case SDLK_LEFT: {
							if (aPressed) {
								aPressed = false;
								SDL_RemoveTimer(slideLTimer);
							}
							break;
						}
						case SDLK_d: case SDLK_RIGHT: {
							if (dPressed) {
								dPressed = false;
								SDL_RemoveTimer(slideRTimer);
							}
							break;
						}
						case SDLK_s : case SDLK_DOWN : {
							if (sPressed) {
								setDropSpeed();
								sPressed = false;
							}
							break;
						}
					}
				}
			}
		}
		// Update the drop speed.
		setDropSpeed();
		if (!callQue.empty()) {
			Grid::GridFunc func = callQue.front();
			CALL_MEMBER_FN(g, func);
			callQue.pop();
			g.printGrid();
		}
	}
}





} /* namespace tet */

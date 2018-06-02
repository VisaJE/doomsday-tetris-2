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


int Events::getDirection() {
	return direction;
}


Uint32 ticker(Uint32 interval, void * arg) {
	cout << "ticking." << endl;
	Events *event = (Events*)arg;
	if (event->g.lost || event->paused || event->quit) {
		return 0;
	}
	event->callQue.push(&Grid::wholeTick);
	cout << event->callQue.size();
	return event->currentInterval;
}


Uint32 slider(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (event->g.lost || event->paused || event->quit) {
		return 0;
	}
	if (event->getDirection() == -1) {
		event->callQue.push(&Grid::moveL);
	}
	else if (event->getDirection() == 1) {
		event->callQue.push(&Grid::moveR);
	}
	return (Uint32)event->currentInterval*0.2;
}


void Events::setDropSpeed() {
	currentInterval = 1000;
}


void Events::init() {
	paused = false;
	SDL_TimerID timer = SDL_AddTimer(currentInterval, &ticker, this);
	SDL_TimerID slideTimer = SDL_AddTimer(currentInterval*0.2, &slider, this);
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					std::cout << "Quit order..." << std::flush;
					paused = true;
					quit = true;
					SDL_RemoveTimer(timer);
					SDL_RemoveTimer(slideTimer);
					break;
				}
				case SDL_KEYDOWN:  {
					switch (event.key.type) {
						case SDLK_a: case SDLK_LEFT: {
							this->direction = -1;
							break;
						}
						case SDLK_d: case SDLK_RIGHT: {
							this->direction = 1;
							break;
						}
						case SDLK_SPACE: {
							this->callQue.push(&Grid::wholeDrop);
							break;
						}
						case SDLK_s : case SDLK_DOWN : {
							currentInterval /= 3;
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
					switch (event.key.type) {
						case SDLK_a: case SDLK_LEFT: {
							if (direction == -1){direction = 0;}
							break;
						}
						case SDLK_d: case SDLK_RIGHT: {
							if (direction == 1) {direction = 0;}
							break;
						}
						case SDLK_s : case SDLK_DOWN : {
							setDropSpeed();
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
		}
		g.printGrid();
	}
}





} /* namespace tet */

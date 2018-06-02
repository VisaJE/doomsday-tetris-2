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

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

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
	cout << "ticking." << endl;
	Events event = *(Events*)arg;
	if (event.g.lost || event.paused || event.quit) {
		return 0;
	}
	Grid::GridFunc func = &Grid::wholeTick;
	cout << event.callQue.size() << flush;
	event.callQue.push(&Grid::wholeTick);
	cout << event.callQue.size();
	return event.currentInterval;
}


void Events::init() {
	paused = false;
	cout << "New for timer.";
	SDL_TimerID timer = SDL_AddTimer(1000, &ticker, this);
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				std::cout << "Quit order..." << std::flush;
				paused = true;
				quit = true;
				SDL_RemoveTimer(timer);
			}
		}
		if (!callQue.empty()) {
			cout << "reacting" << endl;
//			Grid::GridFunc func = callQue.front();
//			CALL_MEMBER_FN(g, func);
//			callQue.pop();
		}
	}
}





} /* namespace tet */

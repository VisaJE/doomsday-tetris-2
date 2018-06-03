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
	mutex = SDL_CreateMutex();
}

Events::~Events() {
	SDL_UnlockMutex(mutex);
	SDL_DestroyMutex(mutex);
}


Uint32 ticker(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (SDL_LockMutex(event->mutex)==0) {
		if (event->g.lost || event->paused || event->quit) {
			return 0;
			SDL_UnlockMutex(event->mutex);
		}
		event->callQue.push(&Grid::wholeTick);
		SDL_UnlockMutex(event->mutex);
	}
	return event->currentInterval;
}


Uint32 sliderL(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (SDL_LockMutex(event->mutex) == 0) {
		if (event->g.lost || event->paused || event->quit) {
			SDL_UnlockMutex(event->mutex);
			return 0;
		}
		event->callQue.push(&Grid::moveL);
		SDL_UnlockMutex(event->mutex);
	}
	return interval;
}


Uint32 sliderR(Uint32 interval, void * arg) {
	Events *event = (Events*)arg;
	if (SDL_LockMutex(event->mutex) == 0) {
		if (event->g.lost || event->paused || event->quit) {
			SDL_UnlockMutex(event->mutex);
			return 0;
		}
		event->callQue.push(&Grid::moveR);
		SDL_UnlockMutex(event->mutex);
	}
	return interval;
}


void Events::setDropSpeed() {
	currentInterval = 1000;
}


int Events::init() {
	setDropSpeed();
	while (!callQue.empty()) {
		callQue.pop();
	}
	paused = false;
	SDL_TimerID timer = SDL_AddTimer(currentInterval, &ticker, this);
	SDL_TimerID slideRTimer;
	SDL_TimerID slideLTimer;
	aPressed = false;
	sPressed = false;
	dPressed = false;
	g.printGrid();
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					std::cout << "Quitting..." << std::flush;
					quit = true;
					SDL_RemoveTimer(timer);
					if (aPressed) {SDL_RemoveTimer(slideLTimer);}
					if (dPressed) {SDL_RemoveTimer(slideRTimer);}
					break;
				}
				case SDL_KEYDOWN:  {
					switch (event.key.keysym.sym) {
						case SDLK_a: case SDLK_LEFT: {
							if (!aPressed) {
								aPressed = true;
								if (SDL_LockMutex(mutex)==0) {
									callQue.push(&Grid::moveL);
									SDL_UnlockMutex(mutex);
								}
								slideLTimer = SDL_AddTimer((Uint32)max((int)(currentInterval*0.08), 40), &sliderL, this);
							}
							break;
						}
						case SDLK_d: case SDLK_RIGHT: {
							if (!dPressed) {
								dPressed = true;
								if (SDL_LockMutex(mutex)==0) {
									callQue.push(&Grid::moveR);
									SDL_UnlockMutex(mutex);
								}
								slideRTimer = SDL_AddTimer((Uint32)max((int)(currentInterval*0.08), 40), &sliderR, this);
							}
							break;
						}
						case SDLK_SPACE: {
							this->callQue.push(&Grid::wholeDrop);
							break;
						}
						case SDLK_s : case SDLK_DOWN : {
							if (!sPressed) {
								if (SDL_LockMutex(mutex) == 0) {
									this->callQue.push(&Grid::wholeTick);
									SDL_UnlockMutex(mutex);
								}
								SDL_RemoveTimer(timer);
								currentInterval = (Uint32)min((int)currentInterval / 2, 100);
								timer =  SDL_AddTimer(currentInterval, &ticker, this);
								sPressed = true;
							}
							break;
						}
						case SDLK_w : case SDLK_UP : {
							if (SDL_LockMutex(mutex)==0) {
								this->callQue.push(&Grid::rotate);
								SDL_UnlockMutex(mutex);
							}
							break;
						}
						case SDLK_p : {
							std::cout << "Pausing..." << std::flush;
							paused = true;
							SDL_RemoveTimer(timer);
							if (aPressed) {SDL_RemoveTimer(slideLTimer);}
							if (dPressed) {SDL_RemoveTimer(slideRTimer);}
							if (pause()) {
								timer = SDL_AddTimer(currentInterval, &ticker, this);
								g.wholeTick();
							}
							break;
						}
						case SDLK_r : {
							SDL_RemoveTimer(timer);
							if (aPressed) {SDL_RemoveTimer(slideLTimer);}
							if (dPressed) {SDL_RemoveTimer(slideRTimer);}
							return 5;
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
								SDL_RemoveTimer(timer);
								setDropSpeed();
								timer =  SDL_AddTimer(currentInterval, &ticker, this);
								sPressed = false;
							}
							break;
						}
					}
				}
			}
		}
		// Update the drop speed.
		if (this->g.droppedAmount()%10 ==9) {
			setDropSpeed();
			if (sPressed) {
				currentInterval = (Uint32)min((int)currentInterval / 2, 100);
			}
		}


		// Do the next thing on call queue.
		if (!callQue.empty() && !g.lost) {
			if (SDL_TryLockMutex(mutex)==0) {
				Grid::GridFunc func = callQue.front();
				CALL_MEMBER_FN(g, func);
				callQue.pop();
				g.printGrid();
				SDL_UnlockMutex(mutex);
			}
		}
	}
	return 0;
}


bool Events::pause() {
	paused = true;
	while (!quit && paused) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					quit = true;
					break;
				}
				case SDL_KEYDOWN:  {
					switch (event.key.keysym.sym) {
						case SDLK_p: {
							paused = false;
							break;
						}
					}
				}
			}
		}
	}
	setDropSpeed();
	return !quit;
}


} /* namespace tet */

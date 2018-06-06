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


Events::Events(Screen *s, Grid *g, int startInterval, int slideSpeed): paused(true), slideSpeed(slideSpeed), g(g), currentInterval(startInterval), startInt(startInterval), screen(s) {
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


Uint32 ticker(Uint32 interval, void * arg) {
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
	currentInterval = startInt - 25*(g->droppedAmount()/10);
}


int Events::menu() {
	screen->menu();
	int err = 0;
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: {
				quit = true;
				break;
			}
			case SDL_KEYDOWN : {
				switch (event.key.keysym.sym)
				case SDLK_RETURN : {
					err = init();
					screen->menu();
					break;
				}
			/*	case SDLK_ESCAPE : {
					quit = true;
					break;
				}*/
			}
			}
		}
	}
	return err;
}


int Events::init() {
	setDropSpeed();
	paused = false;
	SDL_TimerID timer = SDL_AddTimer(startInt, &ticker, this);
	SDL_TimerID slideRTimer;
	SDL_TimerID slideLTimer;
	aPressed = false;
	sPressed = false;
	dPressed = false;
//	g.printGrid();
	screen->printGrid();
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					std::cout << "Quitting..." << std::flush;
					SDL_UnlockMutex(mutex);
					quit = true;
					SDL_RemoveTimer(timer);
					if (aPressed) {SDL_RemoveTimer(slideLTimer);}
					if (dPressed) {SDL_RemoveTimer(slideRTimer);}
					break;
				}
				case SDL_KEYDOWN:  {
					switch (event.key.keysym.sym) {
					// MOVE LEFT
						case SDLK_a: case SDLK_LEFT: {
							if (!aPressed) {
								aPressed = true;
								if (SDL_LockMutex(mutex)==0) {
									callQue.push(&Grid::moveL);
									SDL_UnlockMutex(mutex);
								}
								slideLTimer = SDL_AddTimer((Uint32)max((int)(currentInterval*(1.0/slideSpeed)+40), 60), &sliderL, this);
							}
							break;
						}
						// MOVE RIGHT
						case SDLK_d: case SDLK_RIGHT: {
							if (!dPressed) {
								dPressed = true;
								if (SDL_LockMutex(mutex)==0) {
									callQue.push(&Grid::moveR);
									SDL_UnlockMutex(mutex);
								}
								slideRTimer = SDL_AddTimer((Uint32)max((int)(currentInterval*(1.0/slideSpeed)+40), 60), &sliderR, this);
							}
							break;
						}
						// INSTANT DROP
						case SDLK_SPACE: {
							if (SDL_LockMutex(mutex) == 0) {
								this->callQue.push(&Grid::wholeDrop);
								SDL_UnlockMutex(mutex);
							}
							break;
						}
						// FAST DROP
						case SDLK_s : case SDLK_DOWN : {
							if (!sPressed) {
								if (SDL_LockMutex(mutex) == 0) {
									this->callQue.push(&Grid::wholeTick);
									SDL_UnlockMutex(mutex);
								}
								SDL_RemoveTimer(timer);
								currentInterval = (Uint32)max((int)currentInterval / 6, 20);
								timer =  SDL_AddTimer(currentInterval, &ticker, this);
								sPressed = true;
							}
							break;
						}
						// ROTATE
						case SDLK_w : case SDLK_UP : {
							if (SDL_LockMutex(mutex)==0) {
								this->callQue.push(&Grid::rotate);
								SDL_UnlockMutex(mutex);
							}
							break;
						}
						// PAUSE
						case SDLK_p : {
							paused = true;
							SDL_RemoveTimer(timer);
							if (aPressed) {
								SDL_RemoveTimer(slideLTimer);
								aPressed = false;}
							if (dPressed) {SDL_RemoveTimer(slideRTimer);
								dPressed=false;}
							sPressed = false;
							if (pause()) {
								timer = SDL_AddTimer(currentInterval, &ticker, this);
								g->wholeTick();
//								g.printGrid();
								screen->printGrid();
							}
							break;
						}
						// RESTART
						case SDLK_r : {
							try {
							SDL_RemoveTimer(timer);
							if (aPressed) {
								SDL_RemoveTimer(slideLTimer);
								aPressed = false;}
							if (dPressed) {
								SDL_RemoveTimer(slideRTimer);
								dPressed = false;}
							SDL_LockMutex(mutex);
							while (!callQue.empty()) {callQue.pop();}
							SDL_UnlockMutex(mutex);
							g->reset();
							timer = SDL_AddTimer(startInt, &ticker, this);
//							g.printGrid();
							screen->printGrid();
							} catch (int i) {}
							break;
							}
						// If game over go to menu.
						case SDLK_RETURN : {
							if (g->lost) {
								SDL_UnlockMutex(mutex);
								SDL_RemoveTimer(timer);
								if (aPressed) {SDL_RemoveTimer(slideLTimer);}
								if (dPressed) {SDL_RemoveTimer(slideRTimer);}
								return 0;
							}
							else break;
						}
						// MENU
						case SDLK_ESCAPE : {
							SDL_UnlockMutex(mutex);
							SDL_RemoveTimer(timer);
							if (aPressed) {SDL_RemoveTimer(slideLTimer);}
							if (dPressed) {SDL_RemoveTimer(slideRTimer);}
							return 0;
						}
						// ERROR INFO
						case SDLK_0 : {
							cout << "Debug info:" << endl << "Buttons pressed (a,s,d): " << aPressed << sPressed << dPressed << endl;
							cout << "Current interval: " << currentInterval << " Stack size: " << callQue.size() << endl;
							cout << "Game status (lost, paused, quit)" << g->lost << paused << quit << endl;
							cout << "Points:" << g->getPoints() << " Dropped blocks: " << g->droppedAmount() << endl;
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
					break;
				}
				default : {
				}
			}

		}
		// Update the drop speed.
		setDropSpeed();
		if (sPressed) {
			currentInterval = (Uint32)max((int)currentInterval / 6, 20);
		}


		// Do the next thing on call queue.
		try {
			if (SDL_LockMutex(mutex)==0) {
				if (!callQue.empty() && !g->lost) {
					Grid::GridFunc func = callQue.front();
					CALL_MEMBER_FN(*g, func);
					callQue.pop();
			//				g.printGrid();
					screen->printGrid();

				}
				SDL_UnlockMutex(mutex);
			}
		} catch (int e) {
			SDL_UnlockMutex(mutex);
		}
	}
	return 0;
}


bool Events::pause() {
	paused = true;
	screen->pause();
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

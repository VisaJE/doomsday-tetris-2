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
#include "queue"

#ifndef EVENTS_H_
#define EVENTS_H_

namespace tet {

class Events {
public:
	Events(Screen *screen, Grid *g, const int startInterval, int slideSpeed);
	virtual ~Events();
	bool paused;
	bool quit;
	int slideSpeed;
	bool goingLeft();
	bool goingRight();
	Grid *g;
	int init();
	Uint32 currentInterval;
	SDL_mutex *mutex;
	std::queue<Grid::GridFunc> callQue;

private:
	int startInt;
	SDL_Event event;
	Screen* screen;
	bool sPressed = false;
	bool aPressed = false;
	bool dPressed = false;
	bool pause();
	void setDropSpeed();

};

} /* namespace tet */

#endif /* EVENTS_H_ */

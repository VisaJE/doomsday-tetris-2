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
	Events(Screen screen, Grid g);
	virtual ~Events();
	bool paused;
	bool quit;
	Grid g;
	void init();
	Uint32 currentInterval = 300;
	int getDirection();
	std::queue<Grid::GridFunc> callQue;
private:
	SDL_Event event;
	Screen screen;
	int direction = 0;
	void setDropSpeed();

};

} /* namespace tet */

#endif /* EVENTS_H_ */

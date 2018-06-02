/*
 * MAIN.cpp
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#define SDL_MAIN_HANDLED
#include <iostream>
#include <cstring>
#include <vector>
#include <SDL2/SDL.h>
#include "Screen.h"
#include "Events.h"
#include "Grid.h"

#include "UberBlockifier.h"

using namespace std;
using namespace tet;


int main()  {

	// Initialise SDL

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		cout << "FAIL" << endl;
		return 1;
	}

	// Initialise Screen
	Screen screen = Screen();

	// Initialise game
	cout << "Uber thing" << endl;
	UberBlockifier blockGen(Conf::boardWidth);
	// Game grid
	vector<bool> g(Conf::boardWidth*Conf::boardHeight, false);
	cout << "grid thing" << endl;
	Grid gameGrid = Grid(g, blockGen);

	// Initialise game handler
	Events eventHandler = Events(screen, gameGrid);
	eventHandler.init();
	SDL_Quit();
	cout << "Bye." << endl;
	return 0;
}



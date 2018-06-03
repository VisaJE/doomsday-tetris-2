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

	// Initialize
	Screen screen = Screen();

	UberBlockifier blockGen(Conf::boardWidth);

	vector<bool> g(Conf::boardWidth*Conf::boardHeight, false);
	Grid gameGrid = Grid(g, blockGen);

	Events eventHandler = Events(screen, gameGrid);
	while (eventHandler.init() == 5) {
		cout << "New game"<< endl;
		blockGen = UberBlockifier(Conf::boardWidth);

		vector<bool> g = vector<bool>(Conf::boardWidth*Conf::boardHeight, false);
		gameGrid = Grid(g, blockGen);

		eventHandler.g=gameGrid;
	}

	SDL_Quit();
	cout << "Bye." << endl;
	return 0;
}



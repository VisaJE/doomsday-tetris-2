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
#include "Conf.h"

#include "UberBlockifier.h"

using namespace std;
using namespace tet;


int main()  {

	// Initialise SDL

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		cout << "FAIL" << endl;
		return 1;
	}

	// Configuring
	Conf conf = Conf();
	const int boardHeight = conf.boardHeight;
	const int boardWidth = conf.boardWidth;
	int screenHeight = conf.screenHeight;
	int screenWidth = conf.screenWidth;



	UberBlockifier blockGen(boardWidth);

	StaticBlock b(vector<bool>(boardWidth*boardHeight, false), boardHeight, boardWidth);
	Grid gameGrid = Grid(b, blockGen, boardHeight, boardWidth);

	// Initialise
	Screen screen = Screen(screenHeight, screenWidth, &gameGrid);

	Events eventHandler = Events(&screen, &gameGrid, conf.startInterval);
	eventHandler.init();

	SDL_Quit();
	cout << "Bye." << endl;
	return 0;
}



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
#include "Grid.h"
#include "Conf.h"
#include "StaticBlock.h"
#include "UberBlockifier.h"
#include "Events.h"
#include "GlobalHighscore.h"

using namespace std;
using namespace tet;


int main()  {
	// Initialise SDL

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		cout << "FAIL" << endl;
		return 1;
	}

    //Lets test the glboal highscore system
    // Configuring
	Conf conf = Conf();
	const int boardHeight = conf.boardHeight;
	const int boardWidth = conf.boardWidth;
	int screenHeight = conf.screenHeight;
	int screenWidth = conf.screenWidth;

	UberBlockifier blockGen(boardWidth);
	StaticBlock b(vector<bool>(boardWidth*boardHeight, false), boardHeight, boardWidth);
    bool fastDropInitiated = false;
	Grid gameGrid(b, blockGen, boardHeight, boardWidth, fastDropInitiated);

	// Initialise
	Screen screen(screenHeight, screenWidth, &gameGrid);

	Events eventHandler(&screen, &gameGrid, conf.startInterval, conf.slideSpeed, conf.competitionValid(), fastDropInitiated);
    cout << "Making ghs" << endl;
    GlobalHighscore ghs = GlobalHighscore(eventHandler);
    cout << "Ghs made" << endl;
	eventHandler.menu();

	SDL_Quit();
	return 0;
}



/*
 * Screen.cpp
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#include "Screen.h"
#include "Grid.h"
#include <math.h>
#include "BoxTexture.h"
#include <vector>

namespace tet {

Screen::Screen(int h, int w, Grid g): SCREEN_HEIGHT(h), SCREEN_WIDTH(w), GRID(g) {
	// Available space for the grid.
	topLeft[0] = 0;
	topLeft[1] = 0;
	bottomRight[0] = h-1;
	bottomRight[1] = w-1;
	int hLim = (bottomRight[0]-topLeft[0])/GRID.height;
	int wLim = (bottomRight[1]-topLeft[1])/GRID.width;
	// Setting the parameters based on the limiting dimension.
	if (hLim < wLim) {
		boxSize = hLim;
		bottomRight[0] = topLeft[0] + GRID.height*hLim;
	} else {
		boxSize = wLim;
		bottomRight[1] = topLeft[1] + GRID.width*wLim;
	}
	boxTexture = BoxTexture(boxSize);
	isDestroyed = false;

	window = SDL_CreateWindow("Tetris",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window==NULL)  {
		cout << "Window not initialised." << endl;
		destroy();
		throw 10;
	}


	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture==NULL || renderer == NULL) {
		cout << "Initialising the renderer failed." << endl;
		destroy();
		throw 11;
	}
	buffer = vector<Uint32>(SCREEN_WIDTH*SCREEN_HEIGHT, 0);


	SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
/*	for (int i = 0; i < boxSize;i++) {
		for (int j = 0; j < boxSize; j++) {
			cout << boxTexture.getIntensity(j, i) << " ";
		}
		cout << endl;
	}*/
}

Screen::~Screen() {
	destroy();
}


void Screen::destroy() {
	if (!isDestroyed) {
		if (texture != NULL) {
			SDL_DestroyTexture(texture);
			}
		if (renderer != NULL) {
				SDL_DestroyRenderer(renderer);
			}
		if (window != NULL) {
			SDL_DestroyWindow(window);
		}
	}

	isDestroyed = true;
}


void Screen::refresh() {
	SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void Screen::setPixel(int x, int y, int r, int g, int b) {
	Uint32 color = 0;
	color += r;
	color <<= 8;
	color += g;
	color <<= 8;
	color += b;
	color <<= 8;
	color += 0xFF;
	buffer[y*SCREEN_WIDTH+x] = color;
}

} /* namespace tet */

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

namespace tet {

Screen::Screen() {
	isDestroyed = false;
	window = SDL_CreateWindow("Test",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window==NULL)  {
		destroy();
		throw 10;
	}


	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture==NULL || renderer == NULL) {
		destroy();
		throw 11;
	}
	Uint32 *buffer;
	try {
		buffer = new Uint32[SCREEN_WIDTH*SCREEN_HEIGHT];

	} catch (int e) {
		destroy();
		throw 12;
	}
	memset(buffer, 0xAF, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(Uint32));

	SDL_UpdateTexture(texture, NULL, buffer, SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);


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




} /* namespace tet */

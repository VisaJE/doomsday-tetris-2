/*
 * Screen.cpp
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Screen.h"
#include "Grid.h"
#include <math.h>
#include "BoxTexture.h"
#include <vector>
#include <sstream>

namespace tet {

Screen::Screen(int h, int w, Grid *g): SCREEN_HEIGHT(h), SCREEN_WIDTH(w), GRID(g) {
	TTF_Init();
	// Available space for the grid.
	topLeft[0] = 0;
	topLeft[1] = 0;
	bottomRight[0] = h-1;
	bottomRight[1] = w-1;
	int hLim = (bottomRight[0]-topLeft[0])/GRID->height;
	int wLim = (bottomRight[1]-topLeft[1])/GRID->width;
	// Setting the parameters based on the limiting dimension.
	if (hLim < wLim) {
		boxSize = hLim;
		bottomRight[0] = topLeft[0] + GRID->height*hLim;
		bottomRight[1] = topLeft[1] + GRID->width*hLim;
		horizontal = true;
		if (SCREEN_WIDTH - 135 < bottomRight[1]) {
			wLim = (SCREEN_WIDTH-topLeft[1]-135)/GRID->width;
			boxSize = wLim;
			bottomRight[0] = topLeft[0] + GRID->height*wLim;
			bottomRight[1] = topLeft[1] + GRID->width*wLim;
		}
	} else {
		boxSize = wLim;
		bottomRight[0] = topLeft[0] + GRID->height*wLim;
		bottomRight[1] = topLeft[1] + GRID->width*wLim;
		horizontal = false;
		if (SCREEN_HEIGHT - 70 < bottomRight[0]) {
			hLim = (SCREEN_HEIGHT-topLeft[0]-70)/GRID->height;
			boxSize = hLim;
			bottomRight[0] = topLeft[0] + GRID->height*hLim;
			bottomRight[1] = topLeft[1] + GRID->width*hLim;

		}
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
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture==NULL || renderer == NULL) {
		cout << "Initialising the renderer failed." << endl;
		destroy();
		throw 11;
	}
	buffer = vector<Uint32>(SCREEN_WIDTH*SCREEN_HEIGHT, 0);
	// To make the background prettier.
	for (int y =0; y < SCREEN_HEIGHT; y++) {
		for (int x =0; x < SCREEN_WIDTH; x++) {
			int whiteness = abs(sin(0.1*x)*sin(0.1*y))*50;
			setPixel(y, x, whiteness, whiteness, 0.5*whiteness);
		}
	}



	font = TTF_OpenFont("arial.ttf", 30);
	if (font == NULL) throw 20;
	textColor = {200, 200, 200};
	if (!horizontal) {
		infoRect.x = 0;
		infoRect.y = bottomRight[0] + 5;
		infoRect.w = SCREEN_WIDTH;
		infoRect.h = SCREEN_HEIGHT-bottomRight[0];
	} else {
		infoRect.x = bottomRight[1]+ 10;
		infoRect.y = 0;
		infoRect.w = SCREEN_WIDTH - bottomRight[1];
		infoRect.h = SCREEN_HEIGHT;
	}

	refresh();


}

Screen::~Screen() {
	destroy();
	TTF_Quit();
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
	TTF_CloseFont(font);
	isDestroyed = true;
}


void Screen::refresh() {
	try {
		SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
		SDL_RenderClear(renderer);

		stringstream text;
		if (horizontal) {
			if (GRID->lost) {
				text << "LOST";
			}
			text << "\nPOINTS\n" << GRID->getPoints() << "\n\nLEVEL\n" << GRID->droppedAmount()/10 << "\n";
		}
		else {
			text << "POINTS: " << GRID->getPoints() << "    LEVEL: " << GRID->droppedAmount()/10 << "\n";
			if (GRID->lost) {
				text << "LOST  ";
			}
		}
		string t = text.str();
		const char* finalText = t.c_str();
		SDL_Surface* gameInfo = TTF_RenderText_Blended_Wrapped(font, finalText, textColor, SCREEN_WIDTH);
		SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, gameInfo);
		int texW = 0;
		int texH = 0;
		SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
		infoRect.w = texW;
		infoRect.h = texH;
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderCopy(renderer, message, NULL, &infoRect);


		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(message);
		SDL_FreeSurface(gameInfo);
	} catch (int e) {
		cout << "Screen refresh failed" << endl;
	}

}


void Screen::setPixel(int y, int x, int r, int g, int b) {
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

void Screen::printGrid() {
	for (int h = 0; h < GRID->height; h++) {
		for (int w = 0; w < GRID->width; w++) {
			if (GRID->isThere(h, w)) {
				for (int y = 0; y < boxSize; y++) {
					for (int x = 0; x < boxSize; x++) {
						int r = min(0.01*GRID->droppedAmount()*boxTexture.getIntensity(y, x), 250.0);
						int g = boxTexture.getIntensity(y, x);
						int b = 50;
						setPixel(topLeft[0]+h*boxSize+y, topLeft[1]+w*boxSize+x, r ,g ,b);
					}
				}
			} else {
				for (int y = 0; y < boxSize; y++) {
					for (int x = 0; x < boxSize; x++) {
						int r = 10;
						int g =  10;
						int b = 50;
						setPixel(topLeft[0]+h*boxSize+y, topLeft[1]+w*boxSize+x, r ,g ,b);
					}
				}

			}
		}
	}
	refresh();

}

void Screen::pause() {
	for (int y = topLeft[0]; y < bottomRight[0]; y++) {
		for (int x = topLeft[1]; x < bottomRight[1]; x++) {
			setPixel(y, x, 10, 10, 50);
		}
	}
	SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderClear(renderer);

	stringstream text;

	if (horizontal) {
		text << "PAUSED\nPOINTS\n" << GRID->getPoints() << "\n\nLEVEL\n" << GRID->droppedAmount()/10 << "\n";
	}
	else {
		text << "POINTS: " << GRID->getPoints() << "    LEVEL: " << GRID->droppedAmount()/10 << "\nPAUSED";
	}
	string t = text.str();
	const char* finalText = t.c_str();
	SDL_Surface* gameInfo = TTF_RenderText_Blended_Wrapped(font, finalText, textColor, SCREEN_WIDTH);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, gameInfo);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
	infoRect.w = texW;
	infoRect.h = texH;
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderCopy(renderer, message, NULL, &infoRect);


	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(message);
	SDL_FreeSurface(gameInfo);
}

} /* namespace tet */

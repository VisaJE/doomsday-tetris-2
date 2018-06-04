/*
 * Screen.h
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#include "Grid.h"
#include "BoxTexture.h"
#include <vector>
#ifndef SCREEN_H_
#define SCREEN_H_

namespace tet {

class Screen {
public:
	int SCREEN_HEIGHT;
	int SCREEN_WIDTH;
	Grid GRID;
	void destroy();
	void start();
	Screen(int h, int w, Grid g);
	virtual ~Screen();
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	bool isDestroyed;
	void refresh();
	void setPixel(int x, int y, int r, int g, int b);
	int topLeft[2]; // (y, x)
	int bottomRight[2];
	int boxSize;
	BoxTexture boxTexture;
	std::vector<Uint32> buffer;
};

} /* namespace tet */

#endif /* SCREEN_H_ */

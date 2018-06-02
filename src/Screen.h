/*
 * Screen.h
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#ifndef SCREEN_H_
#define SCREEN_H_

namespace tet {

class Screen {
public:
	static const int SCREEN_WIDTH = 400;
	static const int SCREEN_HEIGHT = 800;
	void destroy();
	void start();
	Screen();
	virtual ~Screen();
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	bool isDestroyed;

};

} /* namespace tet */

#endif /* SCREEN_H_ */

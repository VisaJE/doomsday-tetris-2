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
#include <string>
#include <sstream>
#include <iomanip>

namespace tet {

Screen::Screen(int h, int w, Grid *g): SCREEN_HEIGHT(h), SCREEN_WIDTH(w), GRID(g) {
	TTF_Init();
	// Available space for the grid.
	topLeft[0] = 15;
	topLeft[1] = 15;
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

	window = SDL_CreateWindow("DoomsdayTetris-2",
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
	textFont = TTF_OpenFont("arial.ttf", boxSize/3.0);
	scoreFont = TTF_OpenFont("arial.ttf", boxSize/1.7);

	if (font == NULL && textFont == NULL) {
		cout << "Font not found" << endl;
		throw 20;
	}
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
	TTF_CloseFont(textFont);
	TTF_CloseFont(scoreFont);
	isDestroyed = true;
}

bool doomText[] = 	{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 1, 1, 0, 0, 0, 1, 1, 1, 0,
						0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
						0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
						0, 1, 1, 0, 0, 0, 0, 1, 0, 0};


void Screen::printHelp(SDL_Rect rect) {
	const char* text = "Key bindings:\n\nP - pause           R - restart\nS - fast drop       W - rotate\nA,D   - slide        SPACE - immediate drop\nESC   - menu     G - online leaderboards\n\nPress enter.";
	SDL_Surface* textSurf = TTF_RenderUTF8_Blended_Wrapped(textFont, text, textColor, rect.w);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, textSurf);
	int texW, texH = 0;
	SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
	rect.w = min(texW, rect.w);
	rect.h = min(texH, rect.h);

	SDL_RenderCopy(renderer, message, NULL, &rect);
	SDL_DestroyTexture(message);
	SDL_FreeSurface(textSurf);
}


void Screen::menu(string names[10], int scores[10]) {
	SDL_Rect textArea;
	textArea.x = topLeft[1] + 45;
	textArea.y = bottomRight[0] - boxSize*3.6;
	textArea.w = bottomRight[1] - topLeft[1];
	textArea.h = bottomRight[0] - textArea.y;
	for (int y = topLeft[0]; y < bottomRight[0]; y++) {
		for (int x = topLeft[1]; x < bottomRight[1]; x++) {
			int r = 10;
			int g =  10;
			int b = 50;
			setPixel(y, x, r ,g ,b);
		}
	}
	if (GRID->height > 5 && GRID->width > 9) {
		for (int h = 0; h < 5; h++) {
			for (int w = 0; w < 10; w++) {
				if (doomText[h*10+w]) {
					for (int y = 0; y < boxSize; y++) {
						for (int x = 0; x < boxSize; x++) {
							int r = 230*boxTexture.getIntensity(y, x);
							int g = 150*boxTexture.getIntensity(y, x);
							int b = 50;
							setPixel(topLeft[0]+h*boxSize+y, topLeft[1]+w*boxSize+x, r ,g ,b);
						}
					}
				}
			}
		}
	}

	SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	printHelp(textArea);

	SDL_Rect hsArea;
	hsArea.x = topLeft[1] + 45;
	hsArea.y = topLeft[0] + boxSize*5 +  45;
	hsArea.w = bottomRight[1]-45 - hsArea.x;
	hsArea.h = textArea.y - hsArea.y;
	printHS(hsArea, names, scores, "LEADERBOARD");
	SDL_RenderPresent(renderer);
}


void Screen::gScorePanel(string names[10], int scores[10])
{
	SDL_Rect textArea;
	textArea.x = topLeft[1] + 45;
	textArea.y = bottomRight[0] - boxSize*3.6;
	textArea.w = bottomRight[1] - topLeft[1];
	textArea.h = bottomRight[0] - textArea.y;
	for (int y = topLeft[0]; y < bottomRight[0]; y++) {
		for (int x = topLeft[1]; x < bottomRight[1]; x++) {
			int r = 30;
			int g =  30;
			int b = 50;
			setPixel(y, x, r ,g ,b);
		}
	}
	if (GRID->height > 5 && GRID->width > 9) {
		for (int h = 0; h < 5; h++) {
			for (int w = 0; w < 10; w++) {
				if (doomText[h*10+w]) {
					for (int y = 0; y < boxSize; y++) {
						for (int x = 0; x < boxSize; x++) {
							int r = 230*boxTexture.getIntensity(y, x);
							int g = 150*boxTexture.getIntensity(y, x);
							int b = 50;
							setPixel(topLeft[0]+h*boxSize+y, topLeft[1]+w*boxSize+x, r ,g ,b);
						}
					}
				}
			}
		}
	}

	SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	printHelp(textArea);

	SDL_Rect hsArea;
	hsArea.x = topLeft[1] + 45;
	hsArea.y = topLeft[0] + boxSize*5 +  45;
	hsArea.w = bottomRight[1]-45 - hsArea.x;
	hsArea.h = textArea.y - hsArea.y;
	printHS(hsArea, names, scores, "GLOBAL LEADERS");
	SDL_RenderPresent(renderer);
}


void Screen::gameOver(string text) {
	SDL_Rect r;
	r.x = topLeft[1] + 10;
	r.w = bottomRight[1] - 10-r.x;
	r.y = topLeft[0] + 3*boxSize;
	r.h = bottomRight[0] - r.y;

	const char* t = text.c_str();
	SDL_Surface* textSurf = TTF_RenderUTF8_Blended_Wrapped(scoreFont, t, textColor, r.w);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, textSurf);
	int texW, texH = 0;
	SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
	r.w = min(texW, r.w);
	r.h = min(texH, r.h);
	for (int i = r.x; i < r.w+r.x; i++) {
		for (int j = r.y; j < r.h + r.y; j++) {
			setPixel(j, i, 0, 0, 0);
		}
	}
	SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH*sizeof(Uint32));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderCopy(renderer, message, NULL, &r);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(message);
	SDL_FreeSurface(textSurf);

}


void Screen::printHS(SDL_Rect hsArea,string names[10], int scores[10],
        const char* header) {
	stringstream t;
	t << header <<"\n\n";
	for (int i = 0; i < 9; i++) {
        if (names[i].size() > 10)
        {
            cout << "Wrong size found at" << i <<endl;
            cout << names[i] << "!" << endl;
            cout << names[i].size() << endl;
        }
	    t << i+1 << ". " << names[i] << string(11-names[i].size(), ' ') << "   -   " << scores[i] << "\n";
	}
	t << 10 << ". " << names[9] << string(11-names[9].size(), ' ') << " -   " << scores[9] << "\n";
	string  s = t.str();
	const char* text = s.c_str();
	SDL_Surface* textSurf = TTF_RenderUTF8_Blended_Wrapped(scoreFont, text, {160, 200, 50}, hsArea.w);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, textSurf);
	int texW, texH = 0;
	SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
	hsArea.w = min(texW, hsArea.w);
	hsArea.h = min(texH, hsArea.h);
	SDL_RenderCopy(renderer, message, NULL, &hsArea);
	SDL_DestroyTexture(message);
	SDL_FreeSurface(textSurf);
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
		SDL_Surface* gameInfo = TTF_RenderUTF8_Blended_Wrapped(font, finalText, textColor, SCREEN_WIDTH);
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
	SDL_Surface* gameInfo = TTF_RenderUTF8_Blended_Wrapped(font, finalText, textColor, SCREEN_WIDTH);
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

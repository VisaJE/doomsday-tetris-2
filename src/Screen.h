/*
 * Screen.h
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include <iostream>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
    Grid *GRID;
    void destroy();
    void printGrid();
    void pause();
    void menu(string names[10], int scores[10]);
    void gScorePanel(string names[10], int scores[10]);
    void gameOver(string text);
    void changeSize(int h, int w);
    Uint32 windowID;
    Screen(int h, int w, Grid *g);
    virtual ~Screen();
private:
    SDL_Window *window=NULL;
    SDL_Renderer *renderer=NULL;
    SDL_Texture *texture=NULL;
    std::string FONT_PATH;
    bool isDestroyed;
    void setSizes();
    void setFonts(); void setTexture();
    void refresh();
    void printHelp(SDL_Rect rect);
    void printHS(SDL_Rect hsArea,string names[10], int scores[10], const char* header);
    void setPixel(int x, int y, int r, int g, int b);
    int topLeft[2]; // (y, x)
    int bottomRight[2];
    int boxSize;
    BoxTexture boxTexture;
    std::vector<Uint32> buffer;
    TTF_Font* font=NULL;
    TTF_Font* textFont=NULL;
    TTF_Font* scoreFont=NULL;
    SDL_Color textColor;
    SDL_Rect infoRect;
    bool horizontal;
};

} /* namespace tet */

#endif /* SCREEN_H_ */

/*
 * Screen.cpp
 *
 *  Created on: 29.5.2018
 *      Author: Eemil
 */
#include "Screen.h"
#include "Block.h"
#include "BoxTexture.h"
#include "Grid.h"
#include "Log.h"
#include "Paths.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
#include <cstring>
#include <fontconfig/fontconfig.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

namespace tet
{

constexpr Uint32 getColor(int r, int g, int b, int a = 0xFF)
{
    Uint32 color = 0;
    color += r & 0xFF;
    color <<= 8;
    color += g & 0xFF;
    color <<= 8;
    color += b & 0xFF;
    color <<= 8;
    color += a & 0xFF;
    return color;
}

void pad(SDL_Rect &rect, int n)
{
    rect.x += n;
    rect.y += n;
    rect.w -= 2 * n;
    rect.h -= 2 * n;
}

void Screen::setSizes()
{
    // Available space for the grid.
    topLeft[0] = 15;
    topLeft[1] = 15;
    bottomRight[0] = SCREEN_HEIGHT - 1;
    bottomRight[1] = SCREEN_WIDTH - 1;
    int hLim = (bottomRight[0] - topLeft[0]) / GRID->height;
    int hLimRed = (SCREEN_WIDTH - topLeft[1] - 135) / GRID->width;
    int wLim = (bottomRight[1] - topLeft[1]) / GRID->width;
    int wLimRed = (SCREEN_HEIGHT - topLeft[0] - 70) / GRID->height;
    // Setting the parameters based on the limiting dimension.
    if (wLimRed < hLimRed)
    {
        boxSize = hLim;
        bottomRight[0] = topLeft[0] + GRID->height * hLim;
        bottomRight[1] = topLeft[1] + GRID->width * hLim;
        horizontal = true;
        if (SCREEN_WIDTH - 135 < bottomRight[1])
        {
            wLim = (SCREEN_WIDTH - topLeft[1] - 135) / GRID->width;
            boxSize = wLim;
            bottomRight[0] = topLeft[0] + GRID->height * wLim;
            bottomRight[1] = topLeft[1] + GRID->width * wLim;
        }
    }
    else
    {
        boxSize = wLim;
        bottomRight[0] = topLeft[0] + GRID->height * wLim;
        bottomRight[1] = topLeft[1] + GRID->width * wLim;
        horizontal = false;
        if (SCREEN_HEIGHT - 70 < bottomRight[0])
        {
            hLim = (SCREEN_HEIGHT - topLeft[0] - 70) / GRID->height;
            boxSize = hLim;
            bottomRight[0] = topLeft[0] + GRID->height * hLim;
            bottomRight[1] = topLeft[1] + GRID->width * hLim;
        }
    }
    /* SCORES BELOW */
    if (!horizontal)
    {

        blockRect.x = topLeft[1];
        blockRect.y = bottomRight[0];

        infoRect.x = SCREEN_WIDTH / 3;
        infoRect.y = blockRect.y;
        infoRect.w = 2 * SCREEN_WIDTH / 3;
        infoRect.h = (SCREEN_HEIGHT - infoRect.y);

        blockRect.w = min(SCREEN_WIDTH / 6, infoRect.h);
        blockRect.h = blockRect.w;
        pad(infoRect, blockRect.h / 20);
        pad(blockRect, blockRect.h / 20);
    }
    /* SCORE ON RIGHT */
    else
    {
        infoRect.x = bottomRight[1];
        infoRect.y = SCREEN_HEIGHT / 5;
        infoRect.w = SCREEN_WIDTH - infoRect.x;
        infoRect.h = SCREEN_HEIGHT / 3;

        blockRect.x = infoRect.x;
        blockRect.y = topLeft[0];
        blockRect.w = min(SCREEN_WIDTH / 10, infoRect.w);
        blockRect.h = min(SCREEN_WIDTH / 10, blockRect.w);

        pad(infoRect, blockRect.h / 20);
        pad(blockRect, blockRect.h / 20);
    }
    boxTexture = BoxTexture(boxSize);

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

Screen::Screen(int h, int w, Grid *g) : SCREEN_HEIGHT(h), SCREEN_WIDTH(w), GRID(g)
{
    TTF_Init();
    isDestroyed = false;

    FONT_PATH = Paths::findFont("Cantarell-Bold");
    if (FONT_PATH.length() == 0)
        FONT_PATH = Paths::findFont("Arial");
    LOG("Font path: %s\n", FONT_PATH.c_str());

    Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
    window = SDL_CreateWindow("DoomsdayTetris-2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, flags);
    if (window == NULL)
    {
        cout << "Window not initialised." << endl;
        destroy();
        throw 10;
    }

    windowID = SDL_GetWindowID(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        cout << "Initialising the renderer failed." << endl;
        destroy();
        throw 11;
    }
    setSizes();
    setTexture();
    setFonts();

    textColor = {200, 200, 200};
    SDL_Rect screenRect{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    textureFromBuffer(screenRect);
    refresh();
}

void Screen::setFonts()
{
    if (font != NULL)
        TTF_CloseFont(font);
    if (textFont != NULL)
        TTF_CloseFont(textFont);
    if (scoreFont != NULL)
        TTF_CloseFont(scoreFont);
    font = TTF_OpenFont(FONT_PATH.c_str(), 30);
    textFont = TTF_OpenFont(FONT_PATH.c_str(), boxSize / 3.0);
    scoreFont = TTF_OpenFont(FONT_PATH.c_str(), boxSize / 1.7);

    if (font == NULL || textFont == NULL || scoreFont == NULL)
    {
        cout << "Font not found" << endl;
        throw 20;
    }
}

void Screen::setTexture()
{
    if (texture != NULL)
        SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL)
    {
        cout << "Initializing texture failed" << endl;
        cout << "Screen width " << SCREEN_WIDTH << endl;
        cout << "Screen height " << SCREEN_HEIGHT << endl;
        throw 12;
    }
    std::fill(buffer.begin(), buffer.end(), 0);
    buffer.resize(SCREEN_HEIGHT * SCREEN_WIDTH);
    // To make the background prettier.
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            int whiteness = abs(sin(0.1 * x) * sin(0.1 * y)) * 50;
            setPixel(y, x, whiteness, whiteness, 0.5 * whiteness);
        }
    }

    if (blockTexture)
        SDL_DestroyTexture(blockTexture);

    blockTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_STREAMING, blockRect.h, blockRect.w);
    SDL_SetTextureBlendMode(blockTexture, SDL_BLENDMODE_BLEND);
}

void Screen::textureFromBuffer(SDL_Rect &area)
{
    try
    {
        Uint32 *pixels;
        int pitch;
        if (SDL_LockTexture(texture, &area, reinterpret_cast<void **>(&pixels), &pitch))
        {
            std::cout << "Texture lock fail" << std::endl;
            throw 1;
        }

        for (int j = 0; j < area.h; j++)
            for (int i = 0; i < area.w; i++)
                pixels[j * (pitch / sizeof(Uint32)) + i] =
                    buffer[(area.y + j) * SCREEN_WIDTH + area.x + i];
        SDL_UnlockTexture(texture);
        SDL_RenderCopy(renderer, texture, &area, &area);
    }
    catch (int i)
    {
        cout << "Grid refresh failed" << endl;
    }
}

void Screen::changeSize(int h, int w)
{
    SDL_RenderClear(renderer);
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;
    setSizes();
    setFonts();
    setTexture();
    // SDL_SetWindowSize(window, w, h);
}

Screen::~Screen()
{
    destroy();
    TTF_Quit();
}

void Screen::destroy()
{
    if (!isDestroyed)
    {
        if (texture != nullptr)
            SDL_DestroyTexture(texture);
        if (renderer != nullptr)
            SDL_DestroyRenderer(renderer);
        if (window != nullptr)
            SDL_DestroyWindow(window);
        if (blockTexture != nullptr)
            SDL_DestroyTexture(blockTexture);
    }
    TTF_CloseFont(font);
    TTF_CloseFont(textFont);
    TTF_CloseFont(scoreFont);
    isDestroyed = true;
}

bool doomText[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0,
                   0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0};

void Screen::printHelp(SDL_Rect rect)
{
    const char *text = "Key bindings:\n\nP - pause           R - restart\nS - fast drop       W - "
                       "rotate\nA,D   - slide        SPACE - immediate drop\nESC   - menu     G - "
                       "online leaderboards\n\nPress enter.";
    SDL_Surface *textSurf = TTF_RenderUTF8_Blended_Wrapped(textFont, text, textColor, rect.w);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, textSurf);
    int texW, texH = 0;
    SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
    rect.w = min(texW, rect.w);
    rect.h = min(texH, rect.h);

    SDL_RenderCopy(renderer, message, NULL, &rect);
    SDL_DestroyTexture(message);
    SDL_FreeSurface(textSurf);
}

void Screen::menu(string names[10], int scores[10])
{
    SDL_RenderClear(renderer);
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_Rect textArea{topLeft[1] + 45, static_cast<int>(bottomRight[0] - boxSize * 3.6),
                      bottomRight[1] - topLeft[1], bottomRight[0] - textArea.y};
    for (int y = topLeft[0]; y < bottomRight[0]; y++)
    {
        for (int x = topLeft[1]; x < bottomRight[1]; x++)
        {
            int r = 10;
            int g = 10;
            int b = 50;
            setPixel(y, x, r, g, b);
        }
    }
    if (GRID->height > 5 && GRID->width > 9)
    {
        for (int h = 0; h < 5; h++)
        {
            for (int w = 0; w < 10; w++)
            {
                if (doomText[h * 10 + w])
                {
                    for (int y = 0; y < boxSize; y++)
                    {
                        for (int x = 0; x < boxSize; x++)
                        {
                            int r = 230 * boxTexture.getIntensity(y, x);
                            int g = 150 * boxTexture.getIntensity(y, x);
                            int b = 50;
                            setPixel(topLeft[0] + h * boxSize + y, topLeft[1] + w * boxSize + x, r,
                                     g, b);
                        }
                    }
                }
            }
        }
    }

    SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    printHelp(textArea);

    SDL_Rect hsArea{topLeft[1] + 45, topLeft[0] + boxSize * 5 + 45, bottomRight[1] - 45 - hsArea.x,
                    textArea.y - hsArea.y};
    printHS(hsArea, names, scores, "LEADERBOARD");
    SDL_RenderPresent(renderer);
}

void Screen::gScorePanel(string names[10], int scores[10])
{
    SDL_RenderClear(renderer);
    SDL_Rect textArea{topLeft[1] + 45, static_cast<int>(bottomRight[0] - boxSize * 3.6),
                      bottomRight[1] - topLeft[1], bottomRight[0] - textArea.y};
    for (int y = topLeft[0]; y < bottomRight[0]; y++)
    {
        for (int x = topLeft[1]; x < bottomRight[1]; x++)
        {
            int r = 30;
            int g = 30;
            int b = 50;
            setPixel(y, x, r, g, b);
        }
    }
    if (GRID->height > 5 && GRID->width > 9)
    {
        for (int h = 0; h < 5; h++)
        {
            for (int w = 0; w < 10; w++)
            {
                if (doomText[h * 10 + w])
                {
                    for (int y = 0; y < boxSize; y++)
                    {
                        for (int x = 0; x < boxSize; x++)
                        {
                            int r = 230 * boxTexture.getIntensity(y, x);
                            int g = 150 * boxTexture.getIntensity(y, x);
                            int b = 50;
                            setPixel(topLeft[0] + h * boxSize + y, topLeft[1] + w * boxSize + x, r,
                                     g, b);
                        }
                    }
                }
            }
        }
    }

    SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    printHelp(textArea);

    SDL_Rect hsArea{topLeft[1] + 45, topLeft[0] + boxSize * 5 + 45, bottomRight[1] - 45 - hsArea.x,
                    textArea.y - hsArea.y};
    printHS(hsArea, names, scores, "GLOBAL LEADERS");
    SDL_RenderPresent(renderer);
}

void Screen::gameOver(string text)
{
    SDL_RenderClear(renderer);
    SDL_Rect r;
    r.x = topLeft[1] + 10;
    r.w = bottomRight[1] - 10 - r.x;
    r.y = topLeft[0] + 3 * boxSize;
    r.h = bottomRight[0] - r.y;

    const char *t = text.c_str();
    SDL_Surface *textSurf = TTF_RenderUTF8_Blended_Wrapped(scoreFont, t, textColor, r.w);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, textSurf);
    int texW, texH = 0;
    SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
    r.w = min(texW, r.w);
    r.h = min(texH, r.h);
    for (int i = r.x; i < r.w + r.x; i++)
    {
        for (int j = r.y; j < r.h + r.y; j++)
        {
            setPixel(j, i, 0, 0, 0);
        }
    }
    SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopy(renderer, message, NULL, &r);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(message);
    SDL_FreeSurface(textSurf);
}

void Screen::printHS(SDL_Rect hsArea, string names[10], int scores[10], const char *header)
{
    stringstream t;
    t << header << "\n\n";
    for (int i = 0; i < 9; i++)
    {
        if (names[i].size() > 10)
        {
            cout << "Wrong size found at" << i << endl;
            cout << names[i] << "!" << endl;
            cout << names[i].size() << endl;
        }
        t << i + 1 << ". " << names[i] << string(11 - names[i].size(), ' ') << "   -   "
          << scores[i] << "\n";
    }
    t << 10 << ". " << names[9] << string(11 - names[9].size(), ' ') << " -   " << scores[9]
      << "\n";
    string s = t.str();
    const char *text = s.c_str();
    SDL_Surface *textSurf =
        TTF_RenderUTF8_Blended_Wrapped(scoreFont, text, {160, 200, 50}, hsArea.w);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, textSurf);
    int texW, texH = 0;
    SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
    hsArea.w = min(texW, hsArea.w);
    hsArea.h = min(texH, hsArea.h);
    SDL_RenderCopy(renderer, message, NULL, &hsArea);
    SDL_DestroyTexture(message);
    SDL_FreeSurface(textSurf);
}

void Screen::showBlock(const Block &block)
{
    SDL_RenderClear(renderer);
    try
    {
        Uint32 *pixels;
        int pitch;
        if (SDL_LockTexture(blockTexture, nullptr, reinterpret_cast<void **>(&pixels), &pitch))
        {
            std::cout << "Texture lock fail" << std::endl;
            throw 1;
        }
        pitch /= sizeof(Uint32);

        const auto boxSize = blockRect.w / std::max(block.height, block.width);
        const auto boxTexture = BoxTexture(boxSize);

        for (int i = 0; i < blockRect.w * blockRect.h; i++)
            pixels[i] = getColor(0xff, 0xff, 0xff, 0xff);

        for (int j = 0; j < block.height; j++)
            for (int i = 0; i < block.width; i++)
            {
                if (block.isThere(j, i))
                    for (int x = 0; x < boxSize; x++)
                        for (int y = 0; y < boxSize; y++)
                            pixels[(j * boxSize + y) * pitch + i * boxSize + x] =
                                getColor(boxTexture.getIntensity(y, x) / 2,
                                         boxTexture.getIntensity(y, x), 50);
            }

        SDL_UnlockTexture(blockTexture);
    }
    catch (int i)
    {
        cout << "Grid refresh failed" << endl;
    }
    printGrid();
}

void Screen::refresh()
{
    textureFromBuffer(infoRect);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderCopy(renderer, blockTexture, nullptr, &blockRect);
    try
    {
        stringstream text;
        if (horizontal)
        {
            if (GRID->lost)
            {
                text << "LOST";
            }
            text << "\nPOINTS\n"
                 << GRID->getPoints() << "\n\nLEVEL\n"
                 << GRID->droppedAmount() / 10 << "\n";
        }
        else
        {
            text << "POINTS: " << GRID->getPoints() << "\nLEVEL: " << GRID->droppedAmount() / 10
                 << "\n";
            if (GRID->lost)
            {
                text << "LOST  ";
            }
        }
        string t = text.str();
        const char *finalText = t.c_str();
        SDL_Surface *gameInfo =
            TTF_RenderUTF8_Blended_Wrapped(font, finalText, textColor, infoRect.w);
        SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, gameInfo);
        int texW = 0;
        int texH = 0;
        SDL_QueryTexture(message, NULL, NULL, &texW, &texH);
        SDL_Rect outRect{infoRect.x, infoRect.y, texW, texH};
        auto scale = std::max((float)outRect.w / infoRect.w, 1.0f);
        outRect.w /= scale;
        outRect.h /= scale;

        SDL_RenderCopy(renderer, message, NULL, &outRect);

        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(message);
        SDL_FreeSurface(gameInfo);
    }
    catch (int e)
    {
        cout << "Screen refresh failed" << endl;
    }
}

void Screen::setPixel(int y, int x, int r, int g, int b)
{
    buffer[y * SCREEN_WIDTH + x] = getColor(r, g, b);
}

void Screen::printGrid()
{
    SDL_RenderClear(renderer);
    SDL_SetWindowResizable(window, SDL_FALSE);
    SDL_Rect updateRect{SCREEN_WIDTH, SCREEN_HEIGHT, -SCREEN_WIDTH, -SCREEN_HEIGHT};
    const auto &changes = GRID->getChanges();
    if (changes.empty())
        return;
    for (auto [h, w] : changes)
    {
        // Calculate rectangle of changes
        const int deltaTop = topLeft[0] + h * boxSize - updateRect.y;
        if (deltaTop < 0)
        {
            updateRect.y += deltaTop;
            updateRect.h -= deltaTop;
        }
        const int deltaLeft = topLeft[1] + w * boxSize - updateRect.x;
        if (deltaLeft < 0)
        {
            updateRect.x += deltaLeft;
            updateRect.w -= deltaLeft;
        }
        updateRect.h = max(updateRect.h, topLeft[0] + (h + 1) * boxSize - updateRect.y);
        updateRect.w = max(updateRect.w, topLeft[1] + (w + 1) * boxSize - updateRect.x);

        if (GRID->isThere(h, w))
        {
            for (int y = 0; y < boxSize; y++)
            {
                for (int x = 0; x < boxSize; x++)
                {
                    int r =
                        min(0.01 * GRID->droppedAmount() * boxTexture.getIntensity(y, x), 250.0);
                    int g = boxTexture.getIntensity(y, x);
                    int b = 50;
                    setPixel(topLeft[0] + h * boxSize + y, topLeft[1] + w * boxSize + x, r, g, b);
                }
            }
        }
        else
        {
            for (int y = 0; y < boxSize; y++)
            {
                for (int x = 0; x < boxSize; x++)
                {
                    int r = 10;
                    int g = 10;
                    int b = 50;
                    setPixel(topLeft[0] + h * boxSize + y, topLeft[1] + w * boxSize + x, r, g, b);
                }
            }
        }
    }
    textureFromBuffer(updateRect);
    refresh();
}

void Screen::pause()
{
    SDL_RenderClear(renderer);
    for (int y = topLeft[0]; y < bottomRight[0]; y++)
        for (int x = topLeft[1]; x < bottomRight[1]; x++)
            setPixel(y, x, 10, 10, 50);

    SDL_UpdateTexture(texture, NULL, &buffer[0], SCREEN_WIDTH * sizeof(Uint32));

    stringstream text;

    if (horizontal)
    {
        text << "PAUSED\nPOINTS\n"
             << GRID->getPoints() << "\n\nLEVEL\n"
             << GRID->droppedAmount() / 10 << "\n";
    }
    else
    {
        text << "POINTS: " << GRID->getPoints() << "    LEVEL: " << GRID->droppedAmount() / 10
             << "\nPAUSED";
    }
    string t = text.str();
    const char *finalText = t.c_str();
    SDL_Surface *gameInfo = TTF_RenderUTF8_Blended_Wrapped(font, finalText, textColor, infoRect.w);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, gameInfo);
    int texW, texH;
    SDL_QueryTexture(message, NULL, NULL, &texW, &texH);

    SDL_Rect outRect{infoRect.x, infoRect.y, texW, texH};
    auto scale = std::max((float)outRect.w / infoRect.w, 1.0f);
    outRect.w /= scale;
    outRect.h /= scale;
    scale = std::max((float)outRect.h / infoRect.h, 1.0f);
    outRect.w /= scale;
    outRect.h /= scale;
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopy(renderer, message, NULL, &outRect);

    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(message);
    SDL_FreeSurface(gameInfo);
}

} /* namespace tet */

#DOOMSDAYTETRIS-2

## Introduction
* It's a tetris.
* It also screws up with the player with randomized blocks.

## Dependencies
* SDL2 library
* SDL_TTF, which also comes with it own dependencies.
* Some font which should be named arial.ttf in the source folder.
* Rapidjson

## Configuring
* The configure file allows for some modifications of the game, including the resolution. Try different resolutions at your own risks: not everything is scaled or handled for different screen sizes. 
* Highscores are "hidden" in an encrypted json file. On corruption the game might fail due to an assertion failure in rapidjson library. To fix, delete the highscore list.
* Some changes in the configuration prevent ones score to be counted to the leaderboard. This is merely to preserve fair game.

#DOOMSDAYTETRIS-2

## Introduction
* It's a tetris.
* It also screws up with the player with randomized blocks.

## Dependencies
* SDL2 library
* SDL_TTF, which also comes with it own dependencies.
* Some font which should be named font.ttf in the source folder.
* Rapidjson

## Configuring
* The configure file allows for some modifications of the game, including the resolution. Try different resolutions at your own risks: not everything is scaled or handled for different screen sizes. 
* Highscores are "hidden" in an encrypted json file. On corruption the game might fail due to an assertion failure in rapidjson library. To fix, delete the highscore list.
* Some changes in the configuration prevent ones score to be counted to the leaderboard. This is merely to preserve fair game.
* Sliding speed has to be at least 1. At that speed the block moves sideways once per one tick.
* The default grid/board that also gets scored is 10x19.

## Online leaderboard
* The game now comes with a possibility to connect to an postgresql server to fetch highscore data. The table should have following columns: name: char[10] or varchar[10] NOT NULL, sore
int and uid: char[10], which may be used as the unique identifier.
* The configuration file is created on launching the game and read on every launch.


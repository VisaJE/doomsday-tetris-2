# Spaghetti DOOMSDAYTETRIS
*Warning: This is bad c++ code*

## Installation
1. cmake .
2. cmake --build . -j


## Introduction
* It's a tetris.
* It also screws up with the player with randomized blocks.

## Dependencies
* autotools-dev autoconf automake libtool
* SDL2 library
* SDL2_TTF, which also comes with it own dependencies.
* Rapidjson
* Libpq-fe (enables online leaderboard and required atm.)
* fontconfig

## Configuring
* The configure files are in XDG_CONFIG_HOME (By default ~/.config/DoomsdayTetris)
* The configure file allows for some modifications of the game, such as the resolution and block drop speed.
* Highscores are "hidden" in an encrypted json file.
* Some changes in the configuration prevent ones score to be counted to the leaderboard. This is merely to preserve fair game.
* Sliding speed has to be at least 1. At that speed the block moves sideways once per one tick.
* The default grid/board that also gets scored is 10x19.

## Online leaderboard
* The game now comes with a possibility to connect to an postgresql server to fetch highscore data. The table should have following columns: name: char[10] or varchar[10] NOT NULL, sore
int and uid: char[10], which may be used as the unique identifier.
* The configuration file is created to the config folder when g is pressed in the menu (also opens the leaderboard).

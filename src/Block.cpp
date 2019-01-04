/*
 * Block.cpp
 *
 *  Created on: 30.5.2018
 *      Author: Eemil
 */

#include "Block.h"
#include "Grid.h"
#include <iostream>
#include <math.h>
namespace tet {


Block::Block(int h, int w, vector<bool> g): height(h), width(w), grid(g) {
    mass = 0;
    for (int i=0;i < height*width;i++){
        if (grid[i]) ++mass;
    }
}


Block::~Block() {
}


void Block::rotate() {
    bool newGrid[width*height];
    for (int i=0;i < height;i++) {
        for (int j=0;j<width;j++) {
            newGrid[(width - 1 - j)*height +  i] = grid[i*width + j];
        }
    }
    int temp = height;
    height = width;
    width = temp;
    for (int i=0;i < height*width;i++){
        grid[i] = newGrid[i];
    }

}


void Block::rotateBack() {
    bool newGrid[width*height];
    for (int i=0;i < height;i++) {
        for (int j=0;j<width;j++) {
            newGrid[j*height + height - 1 - i] = grid[i*width + j];
        }
    }
    int temp = height;
    height = width;
    width = temp;
    for (int i=0;i < height*width;i++){
        grid[i] = newGrid[i];
    }
}

// Rounds the value to the nearest int.
void Block::massCenter(int res[2]) {
    double xMas = 0;
    double yMas = 0;
    for (int i=0;i<width;i++) {
        for (int j=0;j<height;j++) {
            if (grid[j*width+i]) {
                xMas += i;
                yMas += j;
            }
        }
    }
    if (mass == 0) {throw 100;}
    res[0] = (int)round(yMas/mass);
    res[1]=(int)round(xMas/mass);
}


bool Block::isThere(int h, int w) {
    return grid[h*width + w];
}
} /* namespace tet */

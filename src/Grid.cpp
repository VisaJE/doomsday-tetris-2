/*
 * Grid.cpp
 *
 *  Created on: 30.5.2018
 *      Author: Eemil
 */

#include "Grid.h"
#include "Block.h"
#include "StaticBlock.h"
#include "UberBlockifier.h"
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;
namespace tet {

// Grid class also handles the grid of the staticBlock. Deleted at the destructor!
Grid::Grid(StaticBlock inp, UberBlockifier uber): staticBlock(inp), blockGen(uber), dropBlock(blockGen.getABlock()), height(Conf::boardHeight), width(Conf::boardWidth) {
	for (int i = 0; i<height*width;i++) {
		grid.push_back(false);
	}
	blockPos[0] = 0;
	blockPos[1] = (width-dropBlock.width)/2;
	refresh();
}

Grid::~Grid() {
}


bool Grid::isThere(int h, int w) {
	return grid[h*width + w];
}


long Grid::getPoints() {
	return points;
}


int Grid::droppedAmount() {
	return blocksDropped;
}

bool Grid::tick() {
	if (dropBlock.height + blockPos[0] >= height) {return false;}
	else if (!staticBlock.tryAdd(dropBlock, blockPos[0]+1, blockPos[1])) {return false;}
	else {
		for (int i=dropBlock.height-1;i>=0;i--) {
			for (int j=0;j<dropBlock.width;j++) {
				if (dropBlock.isThere(i, j)) {
					grid[(i+blockPos[0])*width + j+blockPos[1]] = false;
					grid[(i + 1 + blockPos[0])*width+j+blockPos[1]] = true;
				}
			}
		}
		blockPos[0] += 1;
		return true;
	}
}


void Grid::moveL() {
	if (blockPos[1] > 0) {
		if (staticBlock.tryAdd(dropBlock, blockPos[0], blockPos[1]-1)) {
			for (int i=dropBlock.height-1;i>=0;i--) {
				for (int j=0;j<dropBlock.width;j++) {
					if (dropBlock.isThere(i, j)) {
						grid[(i+blockPos[0])*width +j+blockPos[1]] = false;
						grid[ (i + blockPos[0])*width + j+blockPos[1] - 1] = true;
					}
				}
			}
		}
		blockPos[1] -= 1;
	}
}


void Grid::moveR() {
	if (blockPos[1] + dropBlock.width< Grid::width) {
		if (staticBlock.tryAdd(dropBlock, blockPos[0], blockPos[1]+1)) {
			for (int i=dropBlock.height-1;i>=0;i--) {
				for (int j=dropBlock.width-1;j>=0;j--) {
					if (dropBlock.isThere(i, j)) {
						grid[(i+blockPos[0])*width +j+blockPos[1]] = false;
						grid [(i + blockPos[0])*width+ j+blockPos[1] + 1] = true;
					}
				}
			}
		}
		blockPos[1] += 1;
	}
}


bool Grid::addBlock(Block block) {
	staticBlock.refresh(grid);
	points = pow(staticBlock.trim(), 1.5)*10;
	dropBlock = block;
	blockPos[0] = 0;
	blockPos[1] = (width - block.width)/2;
	if (!staticBlock.tryAdd(dropBlock, 0, blockPos[1])) {return false;}
/*	for (int i=dropBlock.height-1;i>=0;i--) {
		for (int j=dropBlock.width-1;j>=0;j--) {
			if (dropBlock.isThere(i,j)) {
				grid[(i + blockPos[0])*width + j+blockPos[1]] = true;
			}
		}
	}*/
	refresh();
	blocksDropped += 1;
	return true;
}


void Grid::refresh() {
	for (int i=0;i<height*width;i++) {
			grid[i] = false;
	}
	// Adding dropblock
	for (int i=dropBlock.height-1;i>=0;i--) {
		for (int j=dropBlock.width-1;j>=0;j--) {
			if (dropBlock.isThere(i, j)) {
				grid[(i + blockPos[0])*width +j+blockPos[1]] = true;
			}
		}
	}
	// Adding static block
	for (int i = 0; i<height; i++) {
		for (int j = 0; j < width; j++) {
			if(staticBlock.isThere(i, j)) {
				grid[i*width + j] = true;
			}
		}
	}
}


void Grid::rotate() {
	int oldCenter[2];
	dropBlock.massCenter(oldCenter);
	dropBlock.rotate();
	int newCenter[2];
	dropBlock.massCenter(newCenter);
	int yPosFix = oldCenter[0] - newCenter[0];
	int xPosFix = oldCenter[1] - newCenter[1];
	int y = max(blockPos[0] + yPosFix, 0);
	int x = blockPos[1] + xPosFix;
	int sideTreshold = (int)ceil(dropBlock.width/2);
	if (slide(y, x, sideTreshold)) {
		refresh();
	} else {
		dropBlock.rotateBack();
	}
}


// Returns the success of placing the rotated block.
bool Grid::slide(int y, int x, int l) {
	if (y + dropBlock.height < height) {
		for (int r=0; r<=l; r++) {
			if (r + x + dropBlock.width <= width && r + x >= 0 && staticBlock.tryAdd(dropBlock, y, r + x)) {
				blockPos[0] = y;
				blockPos[1] = r + x;
				return true;
			}
			if (r != 0 && x>=r && -r + x + dropBlock.width <= width && staticBlock.tryAdd(dropBlock, y, -r + x)) {
				blockPos[0] = y;
				blockPos[1] = -r + x;
				return true;
			}
		}
	}
	return false;
}


void Grid::wholeDrop() {
	int p = 0;
	while (tick()) {
		p++;
	}
	points += p*5*dropBlock.mass;
}


void Grid::wholeTick() {
	if (!tick()) {
		if (!addBlock(blockGen.getRandom())) {
			lost = true;
		}
	}
	points += dropBlock.mass;
}


void Grid::printGrid() {
	cout << "Block position: (" << blockPos[0] << ", " << blockPos[1] << ")" << endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (isThere(i, j)) {
				cout << 'O';
			} else {
				cout << '*';
			}
		}
		cout << endl;
	}
	cout << "----------" << endl;
}
} /* namespace tet */

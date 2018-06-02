/*
 * StaticBlock.cpp
 *
 *  Created on: 30.5.2018
 *      Author: Eemil
 */

#include "StaticBlock.h"
#include "Conf.h"
#include <iostream>
#include <vector>
using namespace std;
namespace tet {

StaticBlock::StaticBlock(vector<bool> g):grid(g)  {
}


StaticBlock::~StaticBlock() {
}


int StaticBlock::trim() {
	int lines = 0;
	for(int i=0;i<Conf::boardHeight;i++) {
		bool full = true;
		for (int j=0;j<Conf::boardWidth;j++) {
			if (!( grid[i*Conf::boardWidth + j] )) {
				full = false;
			}
		}
		if (full) {
			++lines;
			for (int w=0;w<Conf::boardWidth;w++) {
				for (int h=0;h<i;h++) {
					grid[ (i-h)*Conf::boardWidth + w] = grid[(i-1-h)*Conf::boardWidth + w];
					grid[w] = false;
				}
			}
		}
	}
	return lines;
}

bool StaticBlock::isThere(int y, int x) {
	return grid[ y*Conf::boardWidth + x];
}


bool StaticBlock::tryAdd(Block block, int h, int w) {
	for (int i= block.height - 1; i >= 0; i--) {
		for (int j=0; j<block.width; j++) {
			if (block.isThere(i, j) && grid[ (i+h)*Conf::boardWidth + j+w] ) {return false;}
		}
	}
return true;
}


void StaticBlock::refresh(vector<bool> g) {
	for (int i = 0;i<Conf::boardWidth*Conf::boardHeight;i++) {
		grid[i] = g[i];
	}
}
}

/*
 * Grid.h
 *
 *  Created on: 30.5.2018
 *      Author: Eemil
 */


#ifndef SRC_GRID_H_
#define SRC_GRID_H_

#include "Block.h"
#include "StaticBlock.h"
#include "UberBlockifier.h"
#include <vector>

namespace tet {

class Grid {
public:
	Grid(StaticBlock inp, UberBlockifier uber);
	virtual ~Grid();
	bool lost = false;
	bool isThere(int h, int w);
	bool tick();
	void moveL();
	void moveR();
	void rotate();
	int wholeDrop();
	bool addBlock(Block block);
	void refresh();
	void wholeTick();
	void printGrid();
	typedef void (Grid::*GridFunc)(void);
private:
	StaticBlock staticBlock;
	UberBlockifier blockGen;
	Block dropBlock;
	int height;
	int width;
	vector<bool> grid;
	int blockPos[2]; // (y, x)
	bool slide(int y, int x, int l);

};
} /* namespace tet */

#endif /* SRC_GRID_H_ */

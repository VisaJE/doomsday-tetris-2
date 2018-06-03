/*
 * StaticBlock.h
 *
 *  Created on: 30.5.2018
 *      Author: Eemil
 */

#ifndef SRC_STATICBLOCK_H_
#define SRC_STATICBLOCK_H_
#include "Block.h"
#include "Conf.h"
#include <iostream>
#include <vector>

using namespace std;
namespace tet {

class StaticBlock {
public:
	StaticBlock(vector<bool> g, const int boardHeight, const int boardWidth);
	virtual ~StaticBlock();
	void refresh(vector<bool> g);
	bool isThere(int y, int x);
	int trim(); // returns the number of removed lines
	bool tryAdd(Block block, int h, int w);
	void reset();
private:
	int boardHeight;
	int boardWidth;
	vector<bool> grid;
};

} /* namespace tet */

#endif /* SRC_STATICBLOCK_H_ */

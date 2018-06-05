/*
 * UberBlockifier.h
 *
 *  Created on: 31.5.2018
 *      Author: Eemil
 */

#ifndef SRC_UBERBLOCKIFIER_H_
#define SRC_UBERBLOCKIFIER_H_

#include "Block.h"
#include <random>
#include <iostream>
#include <vector>

using namespace std;
namespace tet {

class UberBlockifier {
public:
	UberBlockifier(int maxSize);
	virtual ~UberBlockifier();
	Block getABlock();
	Block getNormal();
	Block getRandom();
	void test(unsigned int times);
private:
	std::mt19937 generator;
	int maxSize;
	Block premade[7] = {Block(1, 4, vector<bool>(4, true)),
			Block(2, 2, vector<bool>(4, true)),
			Block(2, 3, vector<bool>{true, true, true, true, false, false}),
			Block(2, 3, vector<bool>{true, true, true, false, false, true}),
			Block(2, 3, vector<bool>{true, true, false, false, true, true}),
			Block(2, 3, vector<bool>{false, true, true, true, true, false}),
			Block(2, 3, vector<bool>{true, true, true, false, true, false})
			};
	Block makeRandom();
	vector<bool> trimGrid(vector<bool> in, int size[2]);
	vector<int> findNeighbours(vector<bool> in, int h, int w, int y, int x);
	vector<int> allNeighbours(vector<bool> in, int h, int w);

};

} /* namespace tet */

#endif /* SRC_UBERBLOCKIFIER_H_ */

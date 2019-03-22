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
    Grid(StaticBlock inp, UberBlockifier uber, const int boardHeight, const int boardWidth, bool& isFastDrop);
    virtual ~Grid();
    bool lost = false;
    int height;
    int width;
    bool isThere(int h, int w);
    long getPoints();
    int droppedAmount();
    bool tick();
    void moveL();
    void moveR();
    void rotate();
    void wholeDrop();
    bool addBlock(Block block);
    void refresh();
    void wholeTick();
    void reset();
    void printGrid();
    void clearDelta();
    std::vector<std::pair<int, int>>* getDelta() {return &delta;}
    bool isDeltaUsable() {return useDelta;}
    typedef void (Grid::*GridFunc)(void);
private:
    long points = 0;
    int blocksDropped = 0;
    StaticBlock staticBlock;
    UberBlockifier blockGen;
    Block dropBlock;
    bool& isFastDrop;
    vector<bool> grid;
    vector<std::pair<int, int>> delta;
    bool useDelta;
    int blockPos[2]; // (y, x)
    bool slide(int y, int x, int l);

};
} /* namespace tet */

#endif /* SRC_GRID_H_ */

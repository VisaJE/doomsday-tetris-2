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
#include <functional>

namespace tet
{

template <class T> class DiffGrid : private std::vector<T>
{
    std::vector<std::pair<int, int>> changes;
    int width;

  public:
    using std::vector<T>::operator[];
    using std::vector<T>::begin;
    using std::vector<T>::end;

    DiffGrid<T>() = default;
    DiffGrid<T>(int h, int w) : std::vector<T>(h * w), width{w}
    {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                changes.emplace_back(i, j);
    }

    void setValue(int y, int x, const T value)
    {
        if (value != this[y * width + x])
            changes.emplace_back(y, x);
        this[y * width + x] = value;
    }
    const std::vector<std::pair<int, int>> &getChanges() { return changes; }
    void clearChanges() { return changes.clear(); }
};

class Grid
{
  public:
    Grid(StaticBlock inp, UberBlockifier uber, const int boardHeight, const int boardWidth,
         bool &isFastDrop);
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
    void refresh();
    void wholeTick();
    void reset();
    // Ascii grid
    void printGrid();

    typedef void (Grid::*GridFunc)(void);

    const std::vector<std::pair<int, int>> &getChanges() { return grid.getChanges(); }
    void clearChanges() { return grid.clearChanges(); }

    /* Called when future block gets generated */
    std::function<void(Block&)> onNewBlock;

  private:
    bool addBlock(Block block);

    long points = 0;
    int blocksDropped = 0;
    StaticBlock staticBlock;
    UberBlockifier blockGen;
    Block dropBlock;
    bool &isFastDrop;
    DiffGrid<bool> grid;
    int blockPos[2]; // (y, x)
    bool slide(int y, int x, int l);
};
} /* namespace tet */

#endif /* SRC_GRID_H_ */

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
namespace tet
{

// Grid class also handles the grid of the staticBlock. Deleted at the destructor!
Grid::Grid(StaticBlock inp, UberBlockifier uber, const int boardHeight, const int boardWidth,
           bool &isFastDrop)
    : height(boardHeight), width(boardWidth), staticBlock(inp), blockGen(uber),
      dropBlock(blockGen.getABlock()), isFastDrop(isFastDrop), grid{boardHeight, boardWidth}
{
    blockPos[0] = 0;
    blockPos[1] = (width - dropBlock.width) / 2;
    refresh();
}

Grid::~Grid() {}

bool Grid::isThere(int h, int w) { return grid[h * width + w]; }

long Grid::getPoints() { return points; }

int Grid::droppedAmount() { return blocksDropped; }

bool Grid::tick()
{
    if (dropBlock.height + blockPos[0] >= height)
        return false;
    else if (!staticBlock.tryAdd(dropBlock, blockPos[0] + 1, blockPos[1]))
        return false;

    for (int i = dropBlock.height - 1; i >= 0; i--)
    {
        for (int j = 0; j < dropBlock.width; j++)
        {
            const int oldPos = (i + blockPos[0]) * width + j + blockPos[1];
            const int newPos = (i + 1 + blockPos[0]) * width + j + blockPos[1];
            if (dropBlock.isThere(i, j))
            {
                grid[oldPos] = false;
                grid[newPos] = true;
            }
        }
    }
    blockPos[0] += 1;
    return true;
}

void Grid::moveL()
{
    if (lost)
        return;
    if (blockPos[1] > 0)
    {
        if (staticBlock.tryAdd(dropBlock, blockPos[0], blockPos[1] - 1))
        {
            for (int i = dropBlock.height - 1; i >= 0; i--)
            {
                for (int j = 0; j < dropBlock.width; j++)
                {
                    if (dropBlock.isThere(i, j))
                    {
                        grid[(i + blockPos[0]) * width + j + blockPos[1]] = false;
                        grid[(i + blockPos[0]) * width + j + blockPos[1] - 1] = true;
                    }
                }
            }
            blockPos[1] -= 1;
        }
    }
}

void Grid::moveR()
{
    if (lost)
        return;
    if (blockPos[1] + dropBlock.width < Grid::width)
    {
        if (staticBlock.tryAdd(dropBlock, blockPos[0], blockPos[1] + 1))
        {
            for (int i = dropBlock.height - 1; i >= 0; i--)
            {
                for (int j = dropBlock.width - 1; j >= 0; j--)
                {
                    if (dropBlock.isThere(i, j))
                    {
                        grid[(i + blockPos[0]) * width + j + blockPos[1]] = false;
                        grid[(i + blockPos[0]) * width + j + blockPos[1] + 1] = true;
                    }
                }
            }
            blockPos[1] += 1;
        }
    }
}

bool Grid::addBlock(Block block)
{
    if (!lost)
    {
        if (onNewBlock)
            onNewBlock(blockGen.getFutureBlock());
        staticBlock.refresh(grid);
        points += pow(staticBlock.trim(), 2) * 30;
        dropBlock = block;
        blockPos[0] = 0;
        blockPos[1] = (width - block.width) / 2;
        try
        {
            if (!slide(blockPos[0], blockPos[1], width - block.width))
            {
                return false;
            }
        }
        catch (int e)
        {
            cout << "Error at sliding on adding block." << endl;
            return false;
        }
        refresh();
        blocksDropped += 1;
        return true;
    }
    return false;
}

void Grid::refresh()
{
    if (!lost)
    {
        for (int i = 0; i < height * width; i++)
        {
            grid[i] = false;
        }
        // Adding dropblock
        for (int i = dropBlock.height - 1; i >= 0; i--)
        {
            for (int j = dropBlock.width - 1; j >= 0; j--)
            {
                if (dropBlock.isThere(i, j))
                {
                    grid[(i + blockPos[0]) * width + j + blockPos[1]] = true;
                }
            }
        }
        // Adding static block
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (staticBlock.isThere(i, j))
                {
                    grid[i * width + j] = true;
                }
            }
        }
    }
}

void Grid::rotate()
{
    if (!lost)
    {
        int oldCenter[2];
        dropBlock.massCenter(oldCenter);
        dropBlock.rotate();
        int newCenter[2];
        dropBlock.massCenter(newCenter);
        int yPosFix = oldCenter[0] - newCenter[0];
        int xPosFix = oldCenter[1] - newCenter[1];
        int y = max(blockPos[0] + yPosFix, 0);
        int x = blockPos[1] + xPosFix;
        int sideTreshold = (int)ceil(dropBlock.width / 2.0);
        //     cout << "sliding at most " << sideTreshold << endl;
        if (slide(y, x, sideTreshold))
        {
            refresh();
        }
        else
        {
            dropBlock.rotateBack();
        }
    }
}

// Returns the success of placing the block.
bool Grid::slide(int y, int x, int l)
{
    if (y + dropBlock.height < height)
    {
        for (int r = 0; r <= l; r++)
        {
            if (r + x + dropBlock.width <= width && r + x >= 0 &&
                staticBlock.tryAdd(dropBlock, y, r + x))
            {
                blockPos[0] = y;
                blockPos[1] = r + x;
                return true;
            }
            if (r != 0 && x >= r && -r + x + dropBlock.width <= width &&
                staticBlock.tryAdd(dropBlock, y, -r + x))
            {
                blockPos[0] = y;
                blockPos[1] = -r + x;
                return true;
            }
        }
    }
    return false;
}

void Grid::wholeDrop()
{
    if (!lost)
    {
        int p = 0;
        while (tick())
        {
            p++;
        }
        if (!addBlock(blockGen.getABlock()))
        {
            lost = true;
        }
        points += p * 4 * dropBlock.mass;
    }
}

void Grid::wholeTick()
{
    if (!lost)
    {
        if (!tick())
        {
            if (!addBlock(blockGen.getABlock()))
            {
                lost = true;
            }
        }
        if (isFastDrop)
            points += 2 * dropBlock.mass;
        else
            points += dropBlock.mass;
    }
}

void Grid::reset()
{
    points = 0;
    blocksDropped = 0;
    staticBlock.reset();
    dropBlock = blockGen.getABlock();
    onNewBlock(blockGen.getFutureBlock());
    blockPos[0] = 0;
    blockPos[1] = (width - dropBlock.width) / 2;
    lost = false;
    refresh();
}

void Grid::printGrid()
{
    cout << "Points: " << points << endl;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (isThere(i, j))
            {
                cout << 'O';
            }
            else
            {
                cout << '*';
            }
        }
        cout << endl;
    }
    cout << "----------" << endl;
}
} /* namespace tet */

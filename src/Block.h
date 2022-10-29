/*
 * Block.h
 *
 *  Created on: 30.5.2018
 *      Author: Eemil
 */

#ifndef SRC_BLOCK_H_
#define SRC_BLOCK_H_
#include <vector>
namespace tet {

class Block {
public:
    virtual ~Block();
    Block(int h, int w, std::vector<bool> g);
    Block();
    int height;
    int width;
    bool isThere(int h, int w) const;
    void rotate();
    void rotateBack();
    void massCenter(int res[2]); // Mass center given (y, x)
    int mass;
private:
    std::vector<bool> grid;
};

} /* namespace tet */

#endif /* SRC_BLOCK_H_ */

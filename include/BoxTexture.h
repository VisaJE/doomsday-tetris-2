/*
 * BoxTexture.h
 *
 *  Created on: 4.6.2018
 *      Author: Eemil
 */
#include <vector>

#ifndef BOXTEXTURE_H_
#define BOXTEXTURE_H_

namespace tet {

class BoxTexture {
public:
    BoxTexture();
    BoxTexture(int width);
    virtual ~BoxTexture();
    int getIntensity(int y,int x) const;
private:
    int width;
    std::vector<int> buffer;
};

} /* namespace tet */

#endif /* BOXTEXTURE_H_ */

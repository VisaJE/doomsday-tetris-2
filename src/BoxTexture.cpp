/*
 * BoxTexture.cpp
 *
 *  Created on: 4.6.2018
 *      Author: Eemil
 */

#include "BoxTexture.h"
#include "math.h"
#include <vector>

namespace tet {

BoxTexture::BoxTexture() {
    width = 0;
}

BoxTexture::BoxTexture(int w): width(w) {
    buffer = std::vector<int>(width*width, 0);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0 || i == 0 || j == width - 1 || i == width - 1) {
                buffer[j*width+i] = 60;
            }
            else {
                int here = (-250*(10-i*(i-width+1) - j*(j-width+1))/(10-2*pow((width+1)/2, 2)));
                buffer[j*width+i] = std::max(90, std::min(here, 200));
            }
        }
    }

}

BoxTexture::~BoxTexture() {
}


int BoxTexture::getIntensity(int y, int x) {
    return buffer[y*width+x];
}
} /* namespace tet */

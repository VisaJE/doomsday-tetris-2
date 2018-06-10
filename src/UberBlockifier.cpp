/*
 * UberBlockifier.cpp
 *
 *  Created on: 31.5.2018
 *      Author: Eemil
 */

#include "UberBlockifier.h"
#include "Block.h"
#include <iostream>
#include <stdio.h>
#include <math.h>    /* time */
#include <random>
#include <vector>
#include "Conf.h"
#include <time.h>
using namespace std;

namespace tet {

UberBlockifier::UberBlockifier(int maxSize): generator(time(NULL)), maxSize(maxSize) {
}

UberBlockifier::~UberBlockifier() {

}


Block UberBlockifier::makeRandom() {
	srand (time(NULL));
	normal_distribution<double> distr (sqrt(maxSize)+1, 1.5);
	int a = min((int)(distr(generator)), maxSize);
	a = max(1, a);
	int b = min((int)(distr(generator)), maxSize);
	b = max(1, b);
	vector<bool> bGrid(a*b, false);
	bGrid[a*b/2] = true;
	int blockAmount = min(max(1, (int)(distr(generator)*min(a, b)/1.7)), a*b-1);
	while(blockAmount > 0) {
		vector<int> possibilities = allNeighbours(bGrid, a, b);
		unsigned int index = (rand() % 100)*possibilities.size()/100;
		if (index < 0 || index >= possibilities.size()) {
			bGrid[possibilities[0]] = true;
		} else bGrid[possibilities[index]] = true;
		for (int i = 0; i < a; i++) {
			for (int j = 0; j < b; j++) {
				if (bGrid[i*b+j]) cout << "O";
				else cout << " ";
			}
			cout << endl;
		}
		--blockAmount;
	}
	int dimensions[2] = {a, b};
	bGrid = trimGrid(bGrid, dimensions);

	Block res(dimensions[0], dimensions[1], bGrid);
	if (dimensions[0] > dimensions[1]) {
		res.rotate();
	}
	int cent[2] = {0, 0};
	res.massCenter(cent);
	if (cent[0] >= res.height/2) {
		res.rotate();
		res.rotate();
	}
	return res;
}


vector<bool> UberBlockifier::trimGrid(vector<bool> in, int size[2]) {
		for (int i = size[0]-1; i>=0;i--) {
			bool empty = true;
			for (int j = 0; j<size[1];j++) {
				if (in[i*size[1] + j]) {empty = false;}
			}
			if (empty) {
				in.erase(in.begin() + i*size[1], in.begin() + i*size[1] + size[1]);
				size[0] -= 1;
			}
		}
		for (int i = size[1]-1; i>=0;i--) {
			bool empty = true;
			for (int j=0; j<size[0]; j++) {
				if (in[j*size[1] + i]) {empty = false;}
			}
			if (empty) {
				for (int j=size[0]-1; j>=0; j--) {
					in.erase(in.begin() + j*size[1] + i);
				}
				size[1] -= 1;
			}
		}
		return in;
}


vector<int> UberBlockifier::findNeighbours(vector<bool> v, int h, int w, int y, int x) {
	vector<int> res;
	if (y>0) {
		if (!v[(y-1)*w + x]) {res.push_back((y-1)*w + x);}
	}
	if (y < h-1) {
		if (!v[(y+1)*w+x]) {
			res.push_back((y+1)*w+x);
		}
	}
	if (x > 0) {
		if (!v[y*w+x-1]) {res.push_back(y*w+x-1);}
	}
	if (x < w-1) {
		if (!v[y*w+x+1]) {res.push_back(y*w+x+1);}
	}
	return res;
}


vector<int> UberBlockifier::allNeighbours(vector<bool> v, int h, int w) {
	vector<int> res;
	for (int i = 0; i< h; i++) {
		for (int j=0;j<w; j++) {
				if (v[i*w + j]) {
				vector<int> newNeigh = findNeighbours(v, h, w, i, j);
				if (newNeigh.size() != 0) {
					res.insert(res.end(), newNeigh.begin(), newNeigh.end() );
				}
			}
		}
	}
	return res;
}

// Contains some fixes for random bugs.
Block UberBlockifier::getABlock() {
	int r = (rand() % 100) % 12;
	if (r == 4) {
		try {
			Block b = makeRandom();
			return b;
		}
		catch (int e) {
			cout << "unexpected error in randomizing";
			return makeRandom();
		}
	}
	int a = (rand() % 100) % 7;
	return premade[a];
}


Block UberBlockifier::getNormal() {
	int a = (rand() % 100) % 7;
	return premade[a];
}


Block UberBlockifier::getRandom() {
	return makeRandom();
}


void UberBlockifier::test(unsigned int times) {
	cout << "Testing random generator." << endl;
	unsigned int i = 0;
	while (i < times) {
		try {
			Block b = getRandom();
			if ( (b.width==2 && b.height == 2) && ((b.isThere(0,0) && !b.isThere(0,1)) || (b.isThere(0,1)  && !b.isThere(0,0)))) {
				cout << endl << "MISTAKE-----------------------" << endl;

				cout << "Size: " << b.height << ", " << b.width << endl;
				for (int y = 0; y < b.height; y++) {
					for (int x = 0; x < b.width; x++) {
						if (b.isThere(y, x)) {
							cout << "O";
						}
						else {cout << " ";}
					}
					cout << endl;
				}
				cout << "---- next ----" << endl;
			}
		} catch (int e) {
					cout << "making random failed" << endl;
				}
		i++;
	}

}


} /* namespace tet */

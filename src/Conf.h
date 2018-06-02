/*
 * Conf.h
 *
 *  Created on: 31.5.2018
 *      Author: Eemil
 */

#ifndef SRC_CONF_H_
#define SRC_CONF_H_

namespace tet {

class Conf {
public:
	Conf();
	virtual ~Conf();
	static const int boardWidth = 10;
	static const int boardHeight = 19;
};

} /* namespace tet */

#endif /* SRC_CONF_H_ */

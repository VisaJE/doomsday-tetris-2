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
	int boardWidth;
	int boardHeight;
	int startInterval;
};

} /* namespace tet */

#endif /* SRC_CONF_H_ */

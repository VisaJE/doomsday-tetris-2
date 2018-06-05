/*
 * Conf.h
 *
 *  Created on: 31.5.2018
 *      Author: Eemil
 */

#ifndef SRC_CONF_H_
#define SRC_CONF_H_

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace tet {

class Conf {
public:
	Conf();
	virtual ~Conf();
	int boardWidth;
	int boardHeight;
	int startInterval;
	int screenHeight;
	int screenWidth;
	int slideSpeed;
	bool competitionValid(); // Checks if the parameters are fair.
private:
	// This order has to remain. The size and order affects both validity and setFromFile functions. Also the default has to be modified.
	char const *keys[6] = {"BOARD_WIDTH", "BOARD_HEIGHT", "START_INTERVAL", "SCREEN_WIDTH", "SCREEN_HEIGHT", "SLIDE_SPEED"};
	bool checkValidity(rapidjson::Document *d);
	void fallBack(FILE* *config, rapidjson::Document *defa);
	void setFromFile(rapidjson::Document *params);
};

} /* namespace tet */

#endif /* SRC_CONF_H_ */

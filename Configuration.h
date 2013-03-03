/*
 * Configuration.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <ovsf.h>

using namespace std;

class Configuration {
private:
	Configuration();
public:
	WHCode wcCtrl;
	static Configuration& getInstance() {
		static Configuration *inst;
		if (inst)
			return *inst;
		inst = new Configuration();
		return *inst;
	}
};

#endif /* CONFIGURATION_H_ */

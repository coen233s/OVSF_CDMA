/*
 * MobileStation.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef MOBILESTATION_H_
#define MOBILESTATION_H_

#include "DeviceBase.h"

class MobileStation : public DeviceBase {
public:
	MobileStation(string &name);
	virtual ~MobileStation();
};

#endif /* MOBILESTATION_H_ */

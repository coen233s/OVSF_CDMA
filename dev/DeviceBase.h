/*
 * DeviceBase.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef DEVICEBASE_H_
#define DEVICEBASE_H_

#include <string>
#include <NamedObject.h>

using namespace std;

class DeviceBase : public NamedObject {
public:
	DeviceBase(const string &deviceId);
	virtual ~DeviceBase();

	string &getDeviceId() {
		return getName();
	}
};

#endif /* DEVICEBASE_H_ */

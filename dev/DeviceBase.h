/*
 * DeviceBase.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef DEVICEBASE_H_
#define DEVICEBASE_H_

#include <string>

using namespace std;

class DeviceBase {
private:
	string m_deviceId;
public:
	DeviceBase(const string &deviceId);
	virtual ~DeviceBase();

	string &getDeviceId() {
		return m_deviceId;
	}
};

#endif /* DEVICEBASE_H_ */

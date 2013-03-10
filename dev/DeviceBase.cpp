/*
 * DeviceBase.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <string>
#include "DeviceBase.h"

DeviceBase::DeviceBase(const string &deviceId)
: NamedObject(deviceId)
{
}

DeviceBase::~DeviceBase() {
}

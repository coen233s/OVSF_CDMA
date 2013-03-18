/*
 * Configuration.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <vector>
#include <iostream>
#include "Configuration.h"

using namespace std;

Configuration::Configuration()
{
	numControlChannelPrio = 32;
}

void Configuration::setControlChannelCode(const WHCode& code)
{
    wcCtrl = code;
    cout << "setChannelCode: ";
    wcCtrl.print();
    cout << endl;
}

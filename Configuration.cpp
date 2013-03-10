/*
 * Configuration.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <vector>
#include "Configuration.h"

Configuration::Configuration() {
	vector<int> ctrlCodeVec;
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(-1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(1);
	ctrlCodeVec.push_back(-1);

	wcCtrl = WHCode(ctrlCodeVec);
}

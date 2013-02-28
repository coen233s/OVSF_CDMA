/*
 * RxTxBase.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef RXTXBASE_H_
#define RXTXBASE_H_

#include <string>

using namespace std;

class RxTxBase {
protected:
    string &name;
public:
	RxTxBase(string &name);
	virtual ~RxTxBase();

	// Simulate one time step (one chip period).
	void onTick();
};

#endif /* RXTXBASE_H_ */

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
    string m_name;
public:
	RxTxBase(const string &name);
	virtual ~RxTxBase();

	// Simulate one time step (one chip period).
	void onTick();

	string &getName() {
		return m_name;
	}
};

#endif /* RXTXBASE_H_ */

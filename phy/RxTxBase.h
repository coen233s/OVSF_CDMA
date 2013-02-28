/*
 * RxTxBase.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef RXTXBASE_H_
#define RXTXBASE_H_

class RxTxBase {
public:
	RxTxBase();
	virtual ~RxTxBase();

	// Simulate one time step (one chip period).
	void onTick();
};

#endif /* RXTXBASE_H_ */

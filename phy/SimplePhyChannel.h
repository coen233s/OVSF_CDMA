/*
 * SimplePhyChannel.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef SIMPLEPHYCHANNEL_H_
#define SIMPLEPHYCHANNEL_H_

#include <memory>
#include <vector>
#include "AbsPhyChannel.h"

using namespace std;

class SimplePhyChannel : public AbsPhyChannel {
private:
	vector<Transmitter *> m_Tx;
	vector<Receiver *> m_Rx;
	int tick;
public:
	SimplePhyChannel();
	virtual ~SimplePhyChannel();

	// Attach a receiver to the physical channel, so it can get the signal
	// from the air medium.
	virtual void attachReceiver(Receiver *rx);

	// Attach a receiver to the physical channel, so it can send signal
	// to the air medium.
	virtual void attachTransmitter(Transmitter *tx);

	// Simulate one time step
	virtual void onTick();

	// Run until shouldStop() returns true
	virtual void run(bool (*shouldStop)());
};

#endif /* SIMPLEPHYCHANNEL_H_ */

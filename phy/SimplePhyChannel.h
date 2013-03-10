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

#define DEFAULT_CHIPRATE	1228800		// IS-95

class SimplePhyChannel : public AbsPhyChannel {
private:
	int m_chipRate;
	vector<Transmitter *> m_Tx;
	vector<Receiver *> m_Rx;
public:
	SimplePhyChannel(int chipRate = DEFAULT_CHIPRATE);
	virtual ~SimplePhyChannel();

	virtual int getChipRate() {
		return m_chipRate;
	}

	// Attach a receiver to the physical channel, so it can get the signal
	// from the air medium.
	virtual void attachReceiver(Receiver *rx);

	// Attach a receiver to the physical channel, so it can send signal
	// to the air medium.
	virtual void attachTransmitter(Transmitter *tx);

	virtual void detachReceiver(Receiver *rx);
	virtual void detachTransmitter(Transmitter *tx);

	// Simulate one time step (in chips)
	virtual void onTick(int time);
};

#endif /* SIMPLEPHYCHANNEL_H_ */

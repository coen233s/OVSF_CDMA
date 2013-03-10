/*
 * Transmitter.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include <ovsf.h>
#include <phy/RxTxBase.h>
#include <dat/BitOutQueue.h>
#include "Receiver.h"

class Transmitter : public RxTxBase {
private:
	// Walsh code
	WHCode m_walshCode;
	int m_walshIdx;
	char m_nextChip;
	char m_currentBit; // -1, 0, +1

    // Data queue
    BitOutQueue m_BitQueue;

    // Coupled receiver
    Receiver *m_pCoupledReceiver;

    // Collision Sensing delay (at least 1)
    int m_CSMADelay;

public:
	Transmitter(const string& name);
	virtual ~Transmitter();
	virtual void onTick(int time);

	// Set Walsh code
	void setWalshCode(WHCode &newCode) {
		// Walsh code is copied to avoid memleaks
		m_walshCode = newCode;
		m_walshIdx = 0;
	}

	void clearWalshCode() {
		m_walshCode = WHCode();
	}

	// Called after onTick(). Returns 0, +1 or -1.
	int getChip() {
		return m_nextChip;
	}

	void pushData(unsigned char ch) {
	    m_BitQueue.push(ch);
	}

	void setCSMA(Receiver *pCoupledRx, int delay) {
		m_pCoupledReceiver = pCoupledRx;
		m_CSMADelay = delay;
	}
};

#endif /* TRANSMITTER_H_ */

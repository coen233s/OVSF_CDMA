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

class Transmitter : public RxTxBase {
private:
	// Walsh code
	WHCode *m_pWalshCode;
	int m_walshIdx;
	char m_nextChip;
	char m_currentBit; // -1, 0, +1

    // Data queue
    BitOutQueue m_BitQueue;

public:
	Transmitter(const string& name);
	virtual ~Transmitter();
	virtual void onTick();

	// Set Walsh code
	void setWalshCode(WHCode *newCode) {
		m_pWalshCode = newCode;
		m_walshIdx = 0;
	}

	// Called after onTick(). Returns 0, +1 or -1.
	int getChip() {
		return m_nextChip;
	}

	void pushData(unsigned char ch) {
	    m_BitQueue.push(ch);
	}
};

#endif /* TRANSMITTER_H_ */

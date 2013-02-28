/*
 * Transmitter.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include "../ovsf.h"
#include "RxTxBase.h"

class Transmitter : public RxTxBase {
private:
	// Walsh code
	WHCode *m_pWalshCode;
	int m_walshIdx;
	char m_nextChip;
public:
	Transmitter();
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
};

#endif /* TRANSMITTER_H_ */

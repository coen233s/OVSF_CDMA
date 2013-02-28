/*
 * Receiver.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <string>
#include <vector>
#include <ovsf.h>
#include <phy/RxTxBase.h>
#include <dat/BitInQueue.h>

using namespace std;

class Receiver : public RxTxBase {
private:
	// Last received chip
	int m_LastChip;

	// Set of Walsh codes
	vector<WHCode *> m_WalshCode;

	// Walsh code index
	vector<int> m_WalshIdx;

	// Walsh code inner product
	vector<int> m_WalshDotProd;

	// Data queue
	BitInQueue m_BitQueue;

public:
	Receiver(string &name);
	virtual ~Receiver();
	virtual void onTick();

	void setWalshCode(vector<WHCode *> newCodes);

	// Called before onTick(). Sets the chip value.
	// Chip value can be negative, 0, or positive
	void setChip(int chip) {
		m_LastChip = chip;
	}

	bool hasData() {
	    return !m_BitQueue.empty();
	}

    unsigned char popData() {
        char dat = m_BitQueue.front();
        m_BitQueue.pop();
        return dat;
    }
};

#endif /* RECEIVER_H_ */

/*
 * Receiver.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <iostream>
#include "Receiver.h"

using namespace std;

Receiver::Receiver(string &name)
: RxTxBase(name)
{
}

Receiver::~Receiver() {
}

void Receiver::setWalshCode(vector<WHCode *> newCodes) {
	m_WalshCode = newCodes;
	m_WalshIdx.clear();
	m_WalshIdx.resize(newCodes.size(), 0);
	m_WalshDotProd.clear();
	m_WalshDotProd.resize(newCodes.size(), 0);
}

void Receiver::onTick() {
	size_t i;
	for (i = 0; i < m_WalshCode.size(); i++) {
		int codeLen = m_WalshCode[i]->length();

		m_WalshDotProd[i] += m_LastChip *
				m_WalshCode[i]->getChipBit(m_WalshIdx[i]);

		// Received one bit
		if (++m_WalshIdx[i] >= codeLen) {
			if (m_WalshDotProd[i] == codeLen)
				m_BitQueue.pushBit(1); // received 1
			else if (m_WalshDotProd[i] == -codeLen)
				m_BitQueue.pushBit(0); // received 0
			else if (m_WalshDotProd[i] != 0) {
				cerr << "Warning: " << "Receiver [" << name <<
						"] got unexpected dot product " << m_WalshDotProd[i] <<
						" codeLen: " << codeLen << endl;
			}

			// clear idx & dot product
            m_WalshIdx[i] = 0;
			m_WalshDotProd[i] = 0;
		}
	}
}

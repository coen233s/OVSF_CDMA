/*
 * BitInQueue.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <iostream>
#include "BitInQueue.h"

using namespace std;

BitInQueue::BitInQueue()
: m_bitMask(1)
, m_dataByte(0)
{
}

BitInQueue::~BitInQueue() {
}

void BitInQueue::pushBit(char bit) {
	if (bit)
		m_dataByte |= m_bitMask;

	// byte boundary
	if (m_bitMask & 0x80) {
		m_bitMask = 1;
		push(m_dataByte);
		m_dataByte = 0;
	} else {
		m_bitMask <<= 1;
	}
}

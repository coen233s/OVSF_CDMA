/*
 * BitInQueue.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include "BitInQueue.h"

using namespace std;

BitInQueue::BitInQueue(const string &name, UpdateListener *updateListener)
: NamedObject(name)
, m_updateListener(updateListener)
, m_bitMask(1)
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
		push_back(m_dataByte);

		dout(getName() << " recv: " << hex << showbase <<
				(int)m_dataByte << dec << endl);

		m_dataByte = 0;
		if (m_updateListener)
			m_updateListener->onUpdate(this);
	} else {
		m_bitMask <<= 1;
	}
}

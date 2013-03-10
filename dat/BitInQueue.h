/*
 * BitInQueue.h
 *
 *  Implement a queue taking bits for input. Bit order is little endian.
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef BITINQUEUE_H_
#define BITINQUEUE_H_

#include <deque>
#include <NamedObject.h>
#include "UpdateListener.h"

using namespace std;

class BitInQueue: public deque<unsigned char>,
				  public NamedObject
{
private:
	UpdateListener *m_updateListener;
	unsigned char m_bitMask;
	unsigned char m_dataByte;

public:
	BitInQueue(const string &name, UpdateListener *updateListener = 0);
	virtual ~BitInQueue();

	void pushBit(char bit);
};

#endif /* BITINQUEUE_H_ */

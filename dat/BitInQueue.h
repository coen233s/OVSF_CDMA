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

#include <queue>

using namespace std;

class BitInQueue: public queue<unsigned char> {
private:
	unsigned char m_bitMask;
	unsigned char m_dataByte;

public:
	BitInQueue();
	virtual ~BitInQueue();

	void pushBit(char bit);
};

#endif /* BITINQUEUE_H_ */

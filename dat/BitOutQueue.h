/*
 * BitOutQueue.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef BITOUTQUEUE_H_
#define BITOUTQUEUE_H_

#include <queue>

using namespace std;

class BitOutQueue : public queue<unsigned char> {
private:
	bool m_hasDataInByteBuffer;
	unsigned char m_bitMask;
	unsigned char m_dataByte;

public:
	BitOutQueue();
	virtual ~BitOutQueue();

	bool hasData();
	char popBit();
};

#endif /* BITOUTQUEUE_H_ */

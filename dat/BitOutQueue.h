/*
 * BitOutQueue.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef BITOUTQUEUE_H_
#define BITOUTQUEUE_H_

#include <queue>
#include <NamedObject.h>

using namespace std;

class BitOutQueue : public queue<unsigned char>,
                    public NamedObject
{
private:
    bool m_hasDataInByteBuffer;
    unsigned char m_bitMask;
    unsigned char m_dataByte;

public:
    BitOutQueue(const string &name);
    virtual ~BitOutQueue();

    bool hasData();
    // returns 0 or 1
    char popBit();
};

#endif /* BITOUTQUEUE_H_ */

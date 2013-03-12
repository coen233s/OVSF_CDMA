/*
 * AbsPhyChannel.h
 *
 *  The class AbsPhyChannel defines the interface for physical channel.
 *
 *  The physical channel can be implemented in the same program, or can run
 *  in a separate process communicating over TCP/IP. Because different
 *  implementations may be needed, this class is made abstract.
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef ABSPHYCHANNEL_H_
#define ABSPHYCHANNEL_H_

#include <sim/SimObject.h>
#include "Receiver.h"
#include "Transmitter.h"

class AbsPhyChannel : public SimObject {
public:
    virtual int getChipRate() = 0;
    virtual void attachReceiver(Receiver *rx) = 0;
    virtual void attachTransmitter(Transmitter *tx) = 0;
    virtual void detachReceiver(Receiver *rx) = 0;
    virtual void detachTransmitter(Transmitter *tx) = 0;
};

#endif /* ABSPHYCHANNEL_H_ */

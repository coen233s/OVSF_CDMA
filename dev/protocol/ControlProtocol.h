/*
 * ControlProtocol.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef CONTROLPROTOCOL_H_
#define CONTROLPROTOCOL_H_

#include <dat/UpdateListener.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>

class ControlProtocol : public UpdateListener {
public:
	ControlProtocol();
	virtual ~ControlProtocol();

	virtual void onUpdate(void *arg);	// arg = &Receiver
};

#endif /* CONTROLPROTOCOL_H_ */

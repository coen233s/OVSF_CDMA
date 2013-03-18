/*
 * ControlProtocol.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef CONTROLPROTOCOL_H_
#define CONTROLPROTOCOL_H_

#include <queue>
#include <dat/UpdateListener.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include "ProtocolData.h"

class ControlProtocol : public UpdateListener {
private:
    enum State {
        WAIT_FOR_MAGIC1,
        WAIT_FOR_MAGIC2,
        WAIT_FOR_FRAME,
        WAIT_FOR_DATA,
    };
    Transmitter &m_tx;
    UpdateListener *m_frameListener;
    enum State m_state;
    struct ControlFrame m_frameIn;
    struct ControlFrame m_frameOut;
    int m_dataCnt;
public:
    ControlProtocol(Transmitter &tx, UpdateListener *frameListener = 0);
    virtual ~ControlProtocol();

    // Invoked by Receiver when new data is available
    virtual void onUpdate(void *arg);	// arg = &Receiver

    // For clients
    void sendHandshake(int uid, int rateMin, int rateMax, bool tr);
    void sendTearDown(int uid, bool tr);
    void sendCodeAck(int uid, bool tr);

	// For base station
	void sendDisconect(int uid, bool tr);

private:
    void sendControl(int uid, int rateMin, int rateMax, bool request, bool tr = false,
         bool ack = false, bool c2s = true);
};

#endif /* CONTROLPROTOCOL_H_ */

/*
 * DataProtocol.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef DATAPROTOCOL_H_
#define DATAPROTOCOL_H_

#include <queue>
#include <dat/UpdateListener.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include "ProtocolData.h"

class DataProtocol : public UpdateListener {
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
    struct DataFrame m_frameIn;
    struct DataFrame m_frameOut;
    int m_dataCnt;
public:
    // dframeListener is a Data Frame Listener
    DataProtocol(Transmitter &tx, UpdateListener *dframeListener = 0);
    virtual ~DataProtocol();

    // Invoked by Receiver when new data is available
    virtual void onUpdate(void *arg);   // arg = &Receiver

    // For clients
    void sendFrame(int fromUid, int toUid, int length, char *pData);
};

#endif /* DATAPROTOCOL_H_ */

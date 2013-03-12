#include <iostream>
#include <string>
#include <vector>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include <phy/SimplePhyChannel.h>
#include <dev/BaseStation.h>
#include <dev/MobileStation.h>
#include <sim/Simulator.h>

using namespace std;

// Min UID is 2 (UID 1 is reserved for BaseStation)
#define UID_1	0x02

class MyMobileStation : public MobileStation {
protected:
    int m_state; // 1 - sending data, 2 - sending done
public:
    MyMobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr=true,
            int tickDelay = 0)
    : MobileStation(name, pch, uid, tr, tickDelay)
    , m_state(0)
    { }

    virtual void onTick(int time) {
        MobileStation::onTick(time);

        if (m_state == 0 && m_pDataChannel && m_pDataChannel->m_tx.hasPendingData())
            m_state = 1;
        else if (m_state == 1 && !m_pDataChannel->m_tx.hasPendingData()) {
            m_state = 2;
            terminate();
        }
    }
};

#define MIN_TIME 100000

bool shouldStop(int time, void *arg) {
    BaseStation *bs = reinterpret_cast<BaseStation *>(arg);
    return time > MIN_TIME && bs->getDataConnections() == 0;
}

int main()
{
    Simulator sim;
    SimplePhyChannel pch;
    BaseStation bs(string("BaseStation"), pch);
    MyMobileStation ms(string("Mobile Station #1"), pch, UID_1);

    const int testRate = pch.getChipRate() / 8;

    ms.setRateRange(testRate, testRate);

    sim.addObject(&bs);
    sim.addObject(&ms);
    sim.addObject(&pch);

    sim.run(shouldStop, &bs);

    return 0;
}

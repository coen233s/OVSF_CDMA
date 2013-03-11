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

int main()
{
    Simulator sim;
    SimplePhyChannel pch;
    BaseStation bs(string("BaseStation"), pch);
    MobileStation ms(string("Mobile Station #1"), pch, UID_1);

    sim.addObject(&bs);
    sim.addObject(&ms);
    sim.addObject(&pch);

    const int timeSteps = 50000;
    sim.run(timeSteps);

    return 0;
}

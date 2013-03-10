#include <phy/SimplePhyChannel.h>
#include <dev/BaseStation.h>
#include <dev/MobileStation.h>
#include <sim/Simulator.h>

using namespace std;

#define UID_1	0x01

int main()
{
    Simulator sim;
    SimplePhyChannel pch;
    BaseStation bs(string("BaseStation"), pch);
    MobileStation ms(string("Mobile Station #1"), pch, UID_1);

    sim.addObject(&bs);
    sim.addObject(&ms);
    sim.addObject(&pch);

    const int timeSteps = 5000;
    sim.run(timeSteps);

    return 0;
}

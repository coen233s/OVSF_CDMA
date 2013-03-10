#include <phy/SimplePhyChannel.h>
#include <dev/BaseStation.h>
#include <dev/MobileStation.h>
#include <sim/Simulator.h>

using namespace std;

#define UID_1	0x01

int main(int argc, char* argv)
{
    Simulator sim;
    SimplePhyChannel pch;
    BaseStation bs(string("BaseStation"), pch);
    MobileStation ms(string("Mobile Station #1"), pch, UID_1);

    SimplePhyChannel pch;
    MobileStation ms2(string("Mobile Station #2"), pch, 2);

    sim.addObject(&bs);
    sim.addObject(&ms);
#if 0 // Error: got unexpected dot product
    sim.addObject(&ms2); 
#endif
    sim.addObject(&pch);

    const int timeSteps = 5000;
    sim.run(timeSteps);

    return 0;
}

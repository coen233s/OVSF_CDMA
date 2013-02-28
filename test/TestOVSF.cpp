#include <iostream>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

int main()
{
  OVSFTree t;
  int expand = 8;
  int l = t.expandTree(expand);

  cout << "expand = " << expand << " new len= " << l << endl;
  t.print();
  t.expandTree(9);
  t.print();

  return 0;
}

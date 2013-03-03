#include <iostream>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void test_assign();
void test_2();

int main()
{
  test_2();
  return 0;
}

void test_2()
{
  cout << "Test 2" << endl;
}

void test_assign()
{
  OVSFTree tree;
  tree.assign(3,1,100);
  tree.print();
  tree.assign(1,1,200);
  tree.print();

  WHCode c1;
  tree.peek(3,1,c1);
  WHCode c2;
  tree.peek(1,1,c2);

  cout << "c1 = ";
  c1.print();
  cout << endl;
  cout << "c2 = ";
  c2.print();
  cout << endl;

  tree.release(1,1);
  tree.print();

  tree.release(3,1);
  tree.print();
}

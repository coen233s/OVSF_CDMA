#include <iostream>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void test_assign();
void test_capacity();

int main()
{
  test_capacity();
  return 0;
}

void test_capacity()
{
  Assigner assigner;
  assigner.assignUserId(100,4);
  assigner.print();

  double capacity = assigner.calcCurrentCapacity();
  cout <<" capacity = " << capacity << endl;

  assigner.releaseUserId(100);
  cout <<" capacity = " << assigner.calcCurrentCapacity() << endl;

  int codeLen = assigner.calcShortestFreeCode(2);
  cout <<" codeLen = " << codeLen << endl;

  std::pair<bool,WHCode> result = assigner.assignUserId(200,4);
  if (result.first) {
    cout << "WHCode is ";
    result.second.print();
    cout << endl;
  }
  else {
    cout << "unsuccessful assign this user to WHCode of length 4." << endl;
  }
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

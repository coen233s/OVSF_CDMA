#include <iostream>
#include <assert.h>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>


using namespace std;

///////////////////////////////////////////////////////////////////////////////

void test_assign();
void test_capacity();
void greedy_server_test();
void test_256bit_code();
void test_generate_CDMACode();
void test_hex();

int main()
{
  //test_capacity();
  //greedy_server_test();
  //test_256bit_code();
  //test_generate_CDMACode();
  test_hex();
  return 0;
}

void test_hex()
{
  std::vector<WHCode> codes = CDMA_GenerateCode(8);
  for (size_t i=0; i<codes.size(); i++) {
    cout << i << " : ";
    codes[i].print();
    cout << "=" << codes[i].toByteArray().c_str();
    cout << endl;
  }
}

void test_generate_CDMACode()
{
  cout << "Generate CDMA chip sequence 8-bit" << endl;
  std::vector<WHCode> codes = CDMA_GenerateCode(8);
  for (size_t i=0; i<codes.size(); i++) {
    cout << i << " : ";
    codes[i].print();
    cout << "=" << codes[i].toHexString().c_str();
    cout << endl;
  }
 
  cout << endl << endl;
  cout << "Generate CDMA chip sequence 256-bit" << endl;
  codes = CDMA_GenerateCode(256);
  for (size_t i=0; i<codes.size(); i++) {
    cout << i << " : ";
    codes[i].print();
    cout << "=" << codes[i].toHexString().c_str();
    cout << endl;
  }
  cout << "successfully generate 256 256-bit chip sequence." << endl;
  cout << endl;
  cout << "Test Orthogonal..." << endl;
  for (size_t i=0; i<codes.size(); i++) {
    for (size_t j=0; j<codes.size(); j++) {
      if (i != j) {
	int v = codes[i].dot(codes[j]);
	if (v != 0) {
	  cout << "code:" << i << " is not orthogonal to code: " << j << endl;
	}
      }
    }
  }
  cout << "all codes are orthogonal." << endl;
}

void test_256bit_code()
{
  Assigner assigner;
  
  std::pair<bool,WHCode> result = assigner.assignUserId(100,256);
  if (result.first) {
    cout << "user 100's WHCode = ";
    result.second.print();
    cout << endl;
    cout << "code Len = " << result.second.length() << endl;
  }
  else {
    cout << "fail: user 100 got nothing back." << endl;
  }

}

void greedy_server_test()
{
  Assigner assigner;
  // we will assign the 3 users in the following requests:
  // user 1 wants 1/8 rate = code len = 8
  // user 2 wants 1/4 rate = code len = 4
  // user 3 wants 1/4 rate = code len = 4

  std::pair<bool,WHCode> result = assigner.assignUserId(100,8);
  if (result.first) {
    cout << "user 100's WHCode = ";
    result.second.print();
    cout << endl;
  }
  else {
    cout << "fail: user 100 got nothing back." << endl;
  }
  //assigner.print();

  std::pair<bool,WHCode> result2 = assigner.assignUserId(200,4);
  if (result2.first) {
    cout << "user 200's WHCode = ";
    result2.second.print();
    cout << endl;
  }
  else {
    cout << "fail: user 200 got nothing back." << endl;
  }
  //assigner.print();

  std::pair<bool,WHCode> result3 = assigner.assignUserId(300,4);
  if (result3.first) {
    cout << "user 300's WHCode = ";
    result3.second.print();
    cout << endl;
  }
  else {
    cout << "fail: user 300 got nothing back." << endl;
  }

  cout << "Check if the assignment are correct..." << endl;
  std::vector<std::pair<int,WHCode> > codes = assigner.listUsedCode();
  for (int k=0; k<codes.size(); k++) {
    cout << "User " << codes[k].first << " has WHCode of ";
    codes[k].second.print();
    cout << endl;
  }

  cout << "Current capacity = " << assigner.calcCurrentCapacity() << endl;
  cout << "shortest code you can get is " << assigner.calcShortestFreeCode(2) << endl;

  assigner.print();
  
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

  result = assigner.assignUserId(300,4);
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

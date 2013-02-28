///////////////////////////////////////////////////////////////////////////////
// OVSF module
// - WHCode
// - OVSFTree
//
// Author: COEN233-S
// Feb 2013
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <assert.h>
#include "ovsf.h"

///////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// 
// WHCode
//
///////////////////////////////////////////////////////////////////////////////

void WHCode::print() const 
{
  for (size_t k=0; k<bits.size(); k++) {
    cout << bits[k] << " ";
  }
}

WHCode::WHCode(const WHCode& base, std::vector<int> pattern)
{
  WHCode negCode = base;
  negCode.negate();

  for (size_t k=0; k<pattern.size(); k++) {
    if (pattern[k] > 0) {
      for (int i=0; i<base.length(); i++) {
	bits.push_back(base.getChipBit(i));
      }
    }
    else {
      for (int i=0; i<negCode.length(); i++) {
	bits.push_back(negCode.getChipBit(i));
      }
    }
  } 
}

void WHCode::negate()
{
  for (size_t i=0; i<bits.size(); i++) {
    bits[i] *= -1;
  }
}

bool WHCode::equals(const WHCode& rhs) const
{
  if (length() != rhs.length())
    return false;

  for (size_t i=0; i<bits.size(); i++) {
    if (bits[i] != rhs.bits[i])
      return false;
  }
  return true;
}

int WHCode::length() const
{
  return bits.size();
}

WHCode& WHCode::operator=(const WHCode& rhs)
{
  bits = rhs.bits;
  return *this;
}

int WHCode::getChipBit(size_t index) const
{
  if (index >= bits.size()) {
    assert(false);
    return 0;
  }
  return bits[index];
}

WHCode::WHCode()
{
  bits.push_back(1);
}

WHCode::WHCode(const std::vector<int> initBits)
{
  assert(initBits.size() % 2 == 0);
  if (initBits.size() % 2 == 0) {
    bits = initBits;
  }
}

WHCode::WHCode(const WHCode& other): bits(other.bits)
{
}

WHCode::~WHCode() 
{
}

std::vector<WHCode> WHCode::split(int k) const
{
  std::vector<WHCode> splits;
  if (k > length())
    return splits;

  if (k == length()) {
    splits.push_back(WHCode(*this));
    return splits;
  }
  
  int len = length() / k;
  for (int i=0; i<k; i++) {
    std::vector<int> tmpBits;
    for (int j=i*len; j<i*len+len; j++) {
      tmpBits.push_back(bits[j]);
    }
    splits.push_back(WHCode(tmpBits));
  }
  return splits;
}

int WHCode::dot(const WHCode& rhs) const
{
  assert(length() == rhs.length());
  if (length() != rhs.length())
      return -1;

  int sum = 0;
  for(size_t i=0; i<bits.size(); i++) {
    sum += bits[i] * rhs.bits[i];
  }
  return sum;
}

//////////////////////////////////////////////////////////////////////////////
//
// OVSFTree
//
///////////////////////////////////////////////////////////////////////////////

OVSFTree::OVSFTree()
{
  // index 0 is unused. 
  codes.push_back(WHCode());
  // index 1 is a root node
  codes.push_back(WHCode());
}

OVSFTree::~OVSFTree()
{
}

bool OVSFTree::peek(int level, int nodeId, WHCode& code) const
{
  return false;
}

OVSFTree::NodeInfo OVSFTree::getNodeInfo(int nodeId)
{
  NodeInfo nif;
  Table_T::iterator it = info.find(nodeId);
  if (it == info.end()) {
    // you are free to use this node
    nif.userId = 0;
    nif.nodeId = nodeId;
    nif.blockNodeId = 0; // none
  }
  else {
    nif = it->second;
  }
  return nif;
}

// Return true if assign the userId to the given level,nodeId pair
// Return false if the nodeId is not available either it is used by other users or it is a block code.
bool OVSFTree::assign(int level, int id, int userId)
{
  // you can assign the same userId to multiple nodeId
  int nodeId = (1 << level) + id;
  cout << "assign nodeid of " << nodeId << endl;
  
  // if this node has not allocated, we need to expand the tree first
  if (nodeId > codeCount()) {
    expandTree(nodeId);
  }

  NodeInfo nif = getNodeInfo(nodeId);
  assert(nif.nodeId == nodeId);
  if (nif.userId != 0) {
    return false;
  }

  if (nif.blockNodeId != 0) {
    return false;
  }

  nif.userId = userId;
  info.insert(std::make_pair(nodeId,nif));

  // need to block all descendants and ascestors nodes
  int pindex = nodeId / 2;
  while (pindex > 0) {
    
    pindex = pindex / 2;
  }

  return false;
}

bool OVSFTree::release(int level, int nodeId)
{
  return false;
}

void OVSFTree::print() const
{
  int maxNode = codeCount();
  queue<int> q;
  q.push(1);
  while (!q.empty()) {
    int nodeId = q.front();
    q.pop();
    if (nodeId > maxNode)
      continue;
    
    q.push(nodeId*2);
    q.push(nodeId*2+1);

    cout << "NodeId:" << nodeId 
	 << ",Level=" << log2(nodeId)
	 << ",WH = ";
    codes[nodeId].print();
    cout << endl;
  }
}

int OVSFTree::codeCount() const
{
  return codes.size() - 1;
}

int OVSFTree::usedCodeCount() const
{
  return 0;
}

int OVSFTree::freeCodeCount() const
{
  return 0;
}

int OVSFTree::blockCodeCount() const
{
  return 0;
}

unsigned int OVSFTree::log2(unsigned int v) const
{
  unsigned int shift = 0;
  while (v >>= 1) 
    shift++;
  return shift;
}

int OVSFTree::expandTree(unsigned int size)
{
  // convert size to a multiple of 2
  unsigned int upperbound = 1 << (log2(size)+1);
  cout << "upperbound = " << upperbound << endl;

  if (codeCount() >= upperbound)
    return codeCount();

  int lastNode = codeCount();

  // allocate more space for expanding
  cout << "upperbound = " << upperbound << ": size=" << codeCount() << endl;
  cout << "dif = " << upperbound-codeCount() << endl;
  unsigned extraspace = upperbound - codeCount() - 1;
  for (unsigned int i=0; i<extraspace; i++) {
    codes.push_back(WHCode());
  }
  cout << "newsize = " << codeCount() << endl;

  // construct new WHCode for the expanding nodes
  queue<int> q;
  q.push(1);
  while (!q.empty()) {
    int nodeId = q.front();
    q.pop();
    if (nodeId >= upperbound) 
      continue;

    q.push(nodeId * 2);
    q.push(nodeId * 2 + 1);

    if (nodeId <= lastNode)
      continue;

    WHCode parent = codes[nodeId/2];
    vector<int> pattern;
    if (nodeId % 2) {
      pattern.push_back(1);
      pattern.push_back(-1);
    }
    else {
      pattern.push_back(1);
      pattern.push_back(1);
    }
    codes[nodeId] = WHCode(parent,pattern);
  }
  cout << "upperbound:" << upperbound << ":treesize:" << codeCount() << endl;
  assert(upperbound - 1 == codeCount());
  return upperbound - 1;
}


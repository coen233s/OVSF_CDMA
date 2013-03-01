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

int WHCode::getChipBit(unsigned int index) const
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

OVSFTree::NodeInfo& OVSFTree::NodeInfo::setBlockNodeId(int id) { 
  std::vector<int>::iterator it = std::find(blockNodeId.begin(),
					    blockNodeId.end(),
					    id);
  if (it == blockNodeId.end()) {
    blockNodeId.push_back(id);
  }
  return *this; 
}

OVSFTree::NodeInfo& OVSFTree::NodeInfo::setUserId(int id) { 
  userId = id; 
  return *this; 
}

OVSFTree::NodeInfo& OVSFTree::NodeInfo::setNodeId(int id) { 
  nodeId = id; 
  return *this; 
}

OVSFTree::NodeInfo& OVSFTree::NodeInfo::unsetUserId() {
  userId = 0;
  return *this;
}

OVSFTree::NodeInfo& OVSFTree::NodeInfo::unsetBlockNodeId(int id)
{
  std::vector<int>::iterator it = std::find(blockNodeId.begin(),
					    blockNodeId.end(),
					    id);
  if (it != blockNodeId.end()) {
    blockNodeId.erase(std::remove(blockNodeId.begin(), 
				  blockNodeId.end(), 
				  id),
		      blockNodeId.end());
  }
  return *this;
}

OVSFTree::NodeInfo::NodeInfo(int id): nodeId(id), userId(0) {
}

OVSFTree::NodeInfo::NodeInfo(): userId(0), nodeId(0) {
}
    
OVSFTree::NodeInfo::NodeInfo(const NodeInfo& other): nodeId(other.nodeId),
					   userId(other.userId),
					   blockNodeId(other.blockNodeId) 
{
}

OVSFTree::NodeInfo& OVSFTree::NodeInfo::operator=(const NodeInfo& rhs)
{
  userId = rhs.userId;
  nodeId = rhs.nodeId;
  blockNodeId = rhs.blockNodeId;
  return *this;
}

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

OVSFTree::NodeInfo OVSFTree::getNodeInfo(int nodeId) const
{
  NodeInfo nif;
  Table_T::const_iterator it = nodeInfo.find(nodeId);
  if (it == nodeInfo.end()) {
    // you are free to use this node
    nif.userId = 0;
    nif.nodeId = nodeId;
  }
  else {
    nif = it->second;
  }
  return nif;
}

void OVSFTree::setNodeInfo(const int nodeId, const NodeInfo& info)
{
  Table_T::iterator it = nodeInfo.find(nodeId);
  if (it == nodeInfo.end()) {
    nodeInfo.insert(std::make_pair(nodeId,info));
  }
  else {
    it->second = info;
  }
}

bool OVSFTree::isBlock(int level, int id) const
{
  assert(validate(level,id));
  int nodeId = convertToNodeId(level,id);
  NodeInfo info = getNodeInfo(nodeId);
  return info.isBlockCode();
}

bool OVSFTree::peek(int level, int id, WHCode& code) const
{
  assert(validate(level,id));
  int nodeId = convertToNodeId(level,id);
  if (nodeId >= 1 && nodeId <= codeCount()) {
    code = codes[nodeId];
    return true;
  }
  return false;
}

// Return true if assign the userId to the given level,nodeId pair
// Return false if the nodeId is not available either it is used by other users or it is a block code.
bool OVSFTree::assign(int level, int id, int userId)
{
  assert(validate(level,id));

  // you can assign the same userId to multiple nodeId
  const int nodeId = (1 << level) + id;
  // if this node has not allocated, we need to expand the tree first
  if (nodeId > codeCount()) {
    if (!isAncestorFreeCode(nodeId))
      return false;
    expandTree(nodeId);
  }


  NodeInfo info = getNodeInfo(nodeId);
  assert(info.nodeId == nodeId);
  if (info.isUsedCode()) {
    return false;
  }

  if (info.isBlockCode()) {
    return false;
  }

  info.userId = userId;
  setNodeInfo(nodeId,info);

  // need to block all descendants and ascestors nodes
  int parentNodeId = nodeId / 2;
  while (parentNodeId > 0) {
    NodeInfo tmpInfo = getNodeInfo(parentNodeId);
    tmpInfo.setBlockNodeId(nodeId);
    setNodeInfo(parentNodeId,tmpInfo);
    parentNodeId /= 2;
  }

  // for all existing descendents code has to be blocked as well
  int lastNode = codeCount();
  int childNodeId = nodeId;
  while (childNodeId <= lastNode) {
    childNodeId = childNodeId * 2 + 1;
    NodeInfo tmpInfo1 = getNodeInfo(childNodeId);
    tmpInfo1.setBlockNodeId(nodeId);
    setNodeInfo(childNodeId,tmpInfo1);
    
    childNodeId--;
    NodeInfo tmpInfo2 = getNodeInfo(childNodeId);
    tmpInfo2.setBlockNodeId(nodeId);
    setNodeInfo(childNodeId,tmpInfo2);
  }
  return false;
}

bool OVSFTree::release(int level, int id)
{
  assert(validate(level,id));
  int nodeId = convertToNodeId(level,id);

  if (nodeId > codeCount())
    return false;

  NodeInfo info = getNodeInfo(nodeId);
  if (!info.isUsedCode())
    return false; // cannot release unclaimed code

  info.unsetUserId();
  setNodeInfo(nodeId,info);

  // unblock ancestors
  int parentId = nodeId/2;
  while (parentId > 0) {
    NodeInfo info = getNodeInfo(parentId);
    info.unsetBlockNodeId(nodeId);
    setNodeInfo(parentId,info);
    parentId /= 2;
  }

  // unblock descendant
  int childId = nodeId;
  int lastNode = codeCount();
  while (childId <= lastNode) {
    childId = childId * 2 + 1;
    NodeInfo info1 = getNodeInfo(childId);
    info1.unsetBlockNodeId(nodeId);
    setNodeInfo(childId,info1);

    childId--;
    NodeInfo info2 = getNodeInfo(childId);
    info1.unsetBlockNodeId(nodeId);
    setNodeInfo(childId,info1);
  }
  return false;
}

bool OVSFTree::validate(int level, int nodeId) const
{
  if (level < 0) return false;
  int maxItems = 1 << level;
  return (nodeId < maxItems);
}

int OVSFTree::convertToNodeId(int level, int id) const
{
   return (1 << level) + id;
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
	 << ",WH = "  ;
    codes[nodeId].print();
    
    NodeInfo info = getNodeInfo(nodeId);
    cout << "Block(";
    for (int k=0; k<info.blockNodeId.size(); k++) {
      cout << info.blockNodeId[k] << ",";
    }
    if(info.blockNodeId.empty()) {
      cout << "0";
    }
    cout << ")UserId(" << info.userId << ")";
    cout << endl;
  }
  cout << "FreeCode = " << freeCodeCount() << endl;
  cout << "UsedCode = " << usedCodeCount() << endl;
  cout << "BlkCode  = " << blockCodeCount() << endl;
}

int OVSFTree::codeCount() const
{
  return codes.size() - 1;
}

int OVSFTree::usedCodeCount() const
{
  int s = 0;
  for (int i=1; i<=codeCount(); i++) {
    NodeInfo info = getNodeInfo(i);
    if (info.isUsedCode())
      s++;
  }
  return s;
}

int OVSFTree::freeCodeCount() const
{
  int s = 0;
  for (int i=1; i<=codeCount(); i++) {
    NodeInfo info = getNodeInfo(i);
    if (!info.isUsedCode() && !info.isBlockCode())
      s++;
  }
  return s;
}

int OVSFTree::blockCodeCount() const
{
  int s = 0;
  for (int i=1; i<=codeCount(); i++) {
    NodeInfo info = getNodeInfo(i);
    if (info.isBlockCode())
      s++;
  }
  return s;
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
  
  if (codeCount() >= upperbound)
    return codeCount();

  int lastNode = codeCount();

  // allocate more space for expanding
  unsigned extraspace = upperbound - codeCount() - 1;
  for (unsigned int i=0; i<extraspace; i++) {
    codes.push_back(WHCode());
  }
 
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
  assert(upperbound - 1 == codeCount());
  return upperbound - 1;
}

bool OVSFTree::isAncestorFreeCode(int nodeId) const
{
  int parentNodeId = nodeId / 2;
  while(parentNodeId > 0) {
    NodeInfo info = getNodeInfo(parentNodeId);
    if (info.isBlockCode()) {
      return false;
    }
    parentNodeId /= 2;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////

void test_assign();

///////////////////////////////////////////////////////////////////////////////

//int main()
//{
//  test_assign();
//  return 0;
//}

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


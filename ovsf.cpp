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

OVSFTree::NodeInfo& OVSFTree::NodeInfo::unsetAllBlockNodeId()
{
  blockNodeId.clear();
  return *this;
}

OVSFTree::NodeInfo::NodeInfo(const WHCode& wh_code, int id): code(wh_code), 
							     nodeId(id), 
							     userId(0) 
{
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
  code = rhs.code;
  return *this;
}

///////////////////////////////////////////////////////////////////////////////

OVSFTree::OVSFTree(int initialSize)
{
  // index 0 is unused. 
  nodes.push_back(NodeInfo());
  // index 1 is a root node
  nodes.push_back(NodeInfo());

  expandTree(initialSize);
}

OVSFTree::~OVSFTree()
{
}

bool OVSFTree::isBlock(int level, int id) const
{
  assert(validate(level,id));
  int nodeId = convertToNodeId(level,id);
  return nodes[nodeId].isBlockCode();
}

bool OVSFTree::peek(int level, int id, WHCode& code) const
{
  assert(validate(level,id));
  int nodeId = convertToNodeId(level,id);
  if (nodeId >= 1 && nodeId <= codeCount()) {
    code = nodes[nodeId].getWHCode();
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

  if (nodes[nodeId].isUsedCode()) {
    return false;
  }

  if (nodes[nodeId].isBlockCode()) {
    return false;
  }

  nodes[nodeId].setUserId(userId);
  setAncestorBlockNode(nodeId,true);
  setDescendantBlockNode(nodeId,true);
  return true;
}

void OVSFTree::releaseAll()
{
  for (unsigned int k=1; k<=codeCount(); k++) {
    nodes[k].unsetUserId();
    nodes[k].unsetAllBlockNodeId();
  }
}

bool OVSFTree::releaseUserId(int userId) 
{
  bool bRelease = false;
  for (unsigned int k=1; k<=codeCount(); k++) {
    if (nodes[k].getUserId() == userId) {
      nodes[k].unsetUserId();
      assert(nodes[k].isBlockCode() == false);
      setAncestorBlockNode(k, false);
      setDescendantBlockNode(k, false);
      bRelease = true;
    }
  }
  return bRelease;
}

bool OVSFTree::release(int level, int id)
{
  assert(validate(level,id));
  int nodeId = convertToNodeId(level,id);

  if (nodeId > codeCount())
    return false;

  if (!nodes[nodeId].isUsedCode())
    return false; // cannot release unclaimed code

  nodes[nodeId].unsetUserId();
  setAncestorBlockNode(nodeId,false);
  setDescendantBlockNode(nodeId,false);
  return true;
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
    nodes[nodeId].getWHCode().print();
    
    cout << "Block(";
    for (int k=0; k<nodes[nodeId].blockNodeId.size(); k++) {
      cout << nodes[nodeId].blockNodeId[k] << ",";
    }
    if(nodes[nodeId].blockNodeId.empty()) {
      cout << "0";
    }
    cout << ")UserId(" << nodes[nodeId].userId << ")";
    cout << endl;
  }
  cout << "FreeCode = " << freeCodeCount() << endl;
  cout << "UsedCode = " << usedCodeCount() << endl;
  cout << "BlkCode  = " << blockCodeCount() << endl;
}

int OVSFTree::codeCount() const
{
  return nodes.size() - 1;
}

int OVSFTree::usedCodeCount() const
{
  int s = 0;
  for (int i=1; i<=codeCount(); i++) {
    if (nodes[i].isUsedCode())
      s++;
  }
  return s;
}

int OVSFTree::freeCodeCount() const
{
  int s = 0;
  for (int i=1; i<=codeCount(); i++) {
    if (!nodes[i].isUsedCode() && !nodes[i].isBlockCode())
      s++;
  }
  return s;
}

int OVSFTree::blockCodeCount() const
{
  int s = 0;
  for (int i=1; i<=codeCount(); i++) {
    if (nodes[i].isBlockCode())
      s++;
  }
  return s;
}

bool OVSFTree::isPowerOfTwo(unsigned int v) const
{
  return ((v > 0) && ((v & (v-1)) == 0));
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
    nodes.push_back(NodeInfo());
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

    WHCode parent = nodes[nodeId/2].getWHCode();
    vector<int> pattern;
    if (nodeId % 2) {
      pattern.push_back(1);
      pattern.push_back(-1);
    }
    else {
      pattern.push_back(1);
      pattern.push_back(1);
    }
    nodes[nodeId] = NodeInfo(WHCode(parent,pattern), nodeId);

  }
  assert(upperbound - 1 == codeCount());
  return upperbound - 1;
}

bool OVSFTree::isAncestorFreeCode(int nodeId) const
{
  int parentNodeId = nodeId / 2;
  while(parentNodeId > 0) {
    if (nodes[parentNodeId].isBlockCode()) {
      return false;
    }
    parentNodeId /= 2;
  }
  return true;
}

int OVSFTree::setAncestorBlockNode(int nodeId, bool isBlock)
{
  int parentNodeId = nodeId / 2;
  while(parentNodeId > 0) {
    if (isBlock)
      nodes[parentNodeId].setBlockNodeId(nodeId);
    else
      nodes[parentNodeId].unsetBlockNodeId(nodeId);
    parentNodeId /= 2;
  }
}

int OVSFTree::setDescendantBlockNode(int nodeId, bool isBlock)
{
  int childId = nodeId;
  const int lastNode = codeCount();
  while (childId <= lastNode) {
    childId = childId * 2 + 1;

    if (childId > lastNode)
      break;

    if (isBlock)
      nodes[childId].setBlockNodeId(nodeId);
    else
      nodes[childId].unsetBlockNodeId(nodeId);

    childId--;
    if (childId > lastNode)
      break;

    if (isBlock)
      nodes[childId].setBlockNodeId(nodeId);
    else
      nodes[childId].unsetBlockNodeId(nodeId);
  }
}

std::vector<std::pair<int,WHCode> > OVSFTree::listUsedCode() const
{
  std::vector<std::pair<int,WHCode> > usedCode;
  for (unsigned int k=1; k<=codeCount(); k++) {
    if (nodes[k].isUsedCode()) {
      usedCode.push_back(std::make_pair(nodes[k].getUserId(),
					nodes[k].getWHCode()));
    }
  }
  return usedCode;
}

///////////////////////////////////////////////////////////////////////////////
Assigner::Assigner()
{
}

Assigner::~Assigner()
{
}

std::pair<bool,WHCode> Assigner::assignUserId(int userId, int codeLens)
{
  if (codeLens <= 0) 
    return std::make_pair(false,WHCode());

  if (!tree.isPowerOfTwo(codeLens))
    return std::make_pair(false,WHCode());

  if (codeLens < 2)
    return std::make_pair(false,WHCode());

  // check if the request code length is available
  if (calcShortestFreeCode(codeLens) != codeLens) 
    return std::make_pair(false,WHCode());

  // look for the non-full bucket that has the least capacity
  int lastNode = tree.codeCount();
  unsigned int level = tree.log2(lastNode - 1);
 
  // count capacity for each group
  int count = 0;
  int beginNode = 1 << level;
  int nodeCount = 1 << level;
  int nodePerGroup = nodeCount / 4;
  int capacity[4] = {0,0,0,0};
  for (int n=0; n<4; n++) {
    int offset = n * nodePerGroup;
    for (int k=0; k<nodePerGroup; k++) {
      int nodeId = beginNode + offset + k;
      if (tree.nodes[nodeId].isFreeCode()) {
	capacity[n]++;
      }
    }
  }

  // find the least non-zero capacity
  int bucket = -1;
  int minCapacity = 10000;
  for (int n=0; n<4; n++) {
    if (capacity[n] == 0)
      continue;

    if (capacity[n] < minCapacity) {
      bucket = n;
      minCapacity = capacity[n];
    }
  }
  if (bucket == -1) {
    // not enough capacity
    return std::make_pair(false,WHCode());
  }
  assert(bucket >= 0 && bucket < 4);
  
  // assign userId to that bucket
  int assignLevel = tree.log2(codeLens);
  beginNode = 1 << assignLevel;
  nodeCount = 1 << assignLevel;
  nodePerGroup = nodeCount / 4;
  int offset = bucket * nodePerGroup; 
  for (int k=0; k<nodePerGroup; k++) {
    int nodeId = beginNode + offset + k;
    if (tree.nodes[nodeId].isFreeCode()) {
      bool status = tree.assign(assignLevel,offset+k,userId);
      assert(status);

      WHCode code;
      tree.peek(assignLevel,offset+k,code);
      return std::make_pair(true,code);
    }
  }
  // somehow it fails
  assert(false);
  return std::make_pair(false,WHCode());
}

std::vector<std::pair<bool,WHCode> > Assigner::assignUserIds(const std::vector<int>& userId,
							     const std::vector<int>& codeLens)
{
  std::vector<std::pair<bool,WHCode> > codes;
  if (userId.size() != codeLens.size())
    return codes;

  for (unsigned int k=0; k<userId.size(); k++) {
    codes.push_back(assignUserId(userId[k],codeLens[k]));
  }
  return codes;
}

void Assigner::releaseUserId(int userId)
{
  tree.releaseUserId(userId);
}

void Assigner::releaseAll()
{
  tree.releaseAll();
}

std::vector<std::pair<int, WHCode> > Assigner::listUsedCode() const
{
  return tree.listUsedCode();
}

int Assigner::calcShortestFreeCode(int requestLen) const 
{ 
  assert(tree.isPowerOfTwo(requestLen));
  if (!tree.isPowerOfTwo(requestLen)) 
    return 0;

  int lastNode = tree.codeCount();
  unsigned int lastLevel = tree.log2(lastNode - 1);
  for (unsigned int lv = 2; lv <= lastLevel; lv++) {
    int codeLen = 1 << lv;

    cout << "codeLen = " << codeLen << endl;

    if (codeLen < requestLen)
      continue;

    int beginNode = 1 << lv;
    int nodeCount = 1 << lv;
    
    cout << "beginNode = " << beginNode << " endingNode=" << beginNode + nodeCount << endl;

    for (int k=0; k<nodeCount; k++) {
      if (tree.nodes[beginNode+k].isFreeCode()) {
	return codeLen;
      }
    }
  }
  return 0; 
}

double Assigner::calcCurrentCapacity() const 
{ 
  int lastNode = tree.codeCount();
  unsigned int level = tree.log2(lastNode - 1);
  cout << "level=" << level << endl;

  int count = 0;
  int beginNode = 1 << level;
  for (int k=beginNode; k<=lastNode; k++) {
    if (tree.nodes[k].isFreeCode()) {
      count++;
    }
  }
  cout << "count = " << count << endl;
  cout << "number = " << lastNode - beginNode + 1 << endl;
  return static_cast<double>(count) / 
         static_cast<double>(lastNode - beginNode + 1);
}

void Assigner::print() const
{
  tree.print();
}

bool Assigner::hasExceedCapacity(const std::vector<int>& codeLength)
{
  // Consider a binary partition, we can treat each capacity as a sum of
  // the smallest capacity in this system. If the sum of capacity is <= 1
  // then we can find this configuration by performing code compacting algorithm.
  // However, we are not assigning anyone a capacity of 1 and 1/2.
  
  // if there is any capacity of 1 and 1/2 ( code length of 1 and 2 ).
  // if there is any, reject this request.
  for (size_t k=0; k<codeLength.size(); k++) {
    if (codeLength[k] <= 2) 
      return false;
  }

  double sum = 0.0;
  for (size_t k=0; k<codeLength.size(); k++) {
    sum += (1.0/(double)codeLength[k]);
  }
  return (sum <= 1.0);
}

///////////////////////////////////////////////////////////////////////////////



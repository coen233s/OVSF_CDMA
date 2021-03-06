///////////////////////////////////////////////////////////////////////////////
// OVSF module
// - WHCode
// - OVSFTree
//
// Author: COEN233-S
// Feb 2013
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <assert.h>
#include <math.h>
#include "ovsf.h"

///////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// 
// WHCode
//
///////////////////////////////////////////////////////////////////////////////

WHCode::WHCode(const std::string byteArray)
{
  std::string::const_reverse_iterator rit;
  for (rit=byteArray.rbegin(); rit!=byteArray.rend(); ++rit) {
    int v = *rit;
    for (int i=0; i<8; i++) {
      if (v % 2)
	bits.push_back(1);
      else
	bits.push_back(-1);
      v = v >> 1;
    }
  }
  reverse(bits.begin(),bits.end());
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

bool WHCode::operator==(const WHCode& rhs)
{
  return bits == rhs.bits;
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

bool WHCode::isOrthogonal(const WHCode& rhs) const
{
	if (length() == rhs.length()) {
		return (dot(rhs) == 0);
	}
	const WHCode& longCode = (length() > rhs.length() ) ? *this: rhs;
	const WHCode& shortCode = (length() > rhs.length() ) ? rhs: *this;

	assert(shortCode.length() > 0);
	int fraction = longCode.length() / shortCode.length();

	std::vector<WHCode> splitcodes = longCode.split(fraction);
	for (size_t k=0; k<splitcodes.size(); ++k) {
		assert(splitcodes[k].length() == shortCode.length());
		if (splitcodes[k].dot(shortCode) != 0)
			return false;
	}
	return true;
}

std::string WHCode::toByteArray() const
{
  // WH code is always a power of 2.
  if (bits.size() >= 8) {
    std::string charArray = "";

    for(size_t i=0; i<bits.size(); i+=8) {
      int sum = 0;
      for (size_t j=0; j<8; j++) {
	sum = sum * 2;
	if (bits[i+j] > 0)
	  sum++;
      }
      assert(sum >= 0 && sum < 256);
      charArray += ((char)sum); 
    }
    return charArray;
  }
  else {
    int sum = 0;
    for (size_t j=0; j<bits.size(); j++) {
      sum = sum * 2;
      if (bits[j] > 0)
	sum++;
    }
    char c = (char)sum;
    return std::string(1,c);
  }
}

std::string WHCode::toHexString() const 
{
  static char hexChar[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

  // WH code is always a power of 2.
  if (bits.size() >= 4) {
    std::string hex = "";

    for(size_t i=0; i<bits.size(); i+=4) {
      int sum = 0;
      for (size_t j=0; j<4; j++) {
	sum = sum * 2;
	if (bits[i+j] > 0)
	  sum++;
      }
      assert(sum >= 0 && sum < 16);
      hex += hexChar[sum];
    }
    return hex;
  }
  else if (bits.size() == 2) {
    int s1 = (bits[0] > 0) ? 2: 0;
    int s2 = (bits[1] > 0) ? 1: 0;
    int sum = s1 + s2;
    switch(sum) {
    case 0: return "0";
    case 1: return "4";
    case 2: return "8";
    case 3: return "C";
    }
  }
  else if (bits.size() == 1) {
    return (bits[0] > 0)? "8" : "0";
  }
  return "0";
}

void WHCode::print() const 
{
  for (size_t k=0; k<bits.size(); k++) {
    cout << bits[k] << " ";
  }
}

std::ostream& operator<<(std::ostream& os, const WHCode& wc)
{
  for (size_t k=0; k<wc.bits.size(); k++) {
    os << wc.bits[k] << " ";
  }
  return os;
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

OVSFTree::NodeInfo::NodeInfo(const WHCode& wh_code, int id): 
  userId(0),  
  nodeId(id), 
  code(wh_code)
{
}

OVSFTree::NodeInfo::NodeInfo(int id): userId(0), nodeId(id) {
}

OVSFTree::NodeInfo::NodeInfo(): userId(0), nodeId(0) {
}
    
OVSFTree::NodeInfo::NodeInfo(const NodeInfo& other): 
blockNodeId(other.blockNodeId),
userId(other.userId),
nodeId(other.nodeId),
code(other.code)
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

#define _OVSFTREE_SANITY_CHECK_ENABLE 1

OVSFTree::OVSFTree(int initialSize)
{
  // index 0 is unused. 
  nodes.push_back(NodeInfo());
  // index 1 is a root node
  nodes.push_back(NodeInfo(1));

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
  for (int k=1; k<=codeCount(); k++) {
    nodes[k].unsetUserId();
    nodes[k].unsetAllBlockNodeId();
  }
}

bool OVSFTree::releaseUserId(int userId) 
{
  bool bRelease = false;
  for (int k=1; k<=codeCount(); k++) {
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

int OVSFTree::expandTreeByLevel(unsigned int level)
{
  return expandTree(1 << level);
}

int OVSFTree::expandTree(unsigned int size)
{
  // convert size to a multiple of 2
  int upperbound = 1 << (Math_Log2(size)+1);
  
  if (codeCount() >= upperbound)
    return codeCount();

  int lastNode = codeCount();
  
  // allocate more space for expanding
  unsigned extraspace = upperbound - codeCount() - 1;
  for (unsigned int i=0; i<extraspace; i++) {
    nodes.push_back(NodeInfo());
  }
  
  // construct new WHCode for the expanding nodes
  std::vector<int> usedNodeIds; // a list of nodeId that are in used.

  queue<int> q;
  q.push(1);
  while (!q.empty()) {
    int nodeId = q.front();
    q.pop();
    if (nodeId >= upperbound) 
      continue;

	// Keep tracking the used nodeId
	if (nodeId <= lastNode && nodes[nodeId].isUsedCode())
		usedNodeIds.push_back(nodeId);

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
 
  for(size_t k=0; k<usedNodeIds.size(); k++) {
	  setAncestorBlockNode(usedNodeIds[k],true);
	  setDescendantBlockNode(usedNodeIds[k],true);
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
  return 0;
}

int OVSFTree::setDescendantBlockNode(int nodeId, bool isBlock)
{
  queue<int> q;
  q.push(nodeId * 2);
  q.push(nodeId * 2 + 1);

  const int lastNode = codeCount();
  while (!q.empty()) {
    int childId = q.front();
    q.pop();

    if (childId > lastNode)
      break;

	q.push(childId * 2);
	q.push(childId * 2 + 1);

    if (isBlock)
      nodes[childId].setBlockNodeId(nodeId);
    else
      nodes[childId].unsetBlockNodeId(nodeId);
  }
  return 0;
}

std::vector<std::pair<int,WHCode> > OVSFTree::listUsedCode() const
{
  std::vector<std::pair<int,WHCode> > usedCode;
  for (int k=1; k<=codeCount(); k++) {
    if (nodes[k].isUsedCode()) {
      usedCode.push_back(std::make_pair(nodes[k].getUserId(),
					nodes[k].getWHCode()));
    }
  }
  return usedCode;
}

unsigned int OVSFTree::getTreeLevel() const
{
  return Math_Log2(codeCount() - 1);
}

void OVSFTree::print() const
{
  cout << *this;
}

std::ostream& operator<<(std::ostream& os, const OVSFTree& t)
{
  int maxNode = t.codeCount();
  queue<int> q;
  q.push(1);
  while (!q.empty()) {
    int nodeId = q.front();
    q.pop();
    if (nodeId > maxNode)
      continue;
    
    q.push(nodeId*2);
    q.push(nodeId*2+1);

    os << "NodeId:" << nodeId 
       << ",Level=" << Math_Log2(nodeId)
       << ",WH = "  << t.nodes[nodeId].getWHCode();

    os << "Block(";
    for (size_t k=0; k<t.nodes[nodeId].blockNodeId.size(); k++) {
      os << t.nodes[nodeId].blockNodeId[k] << ",";
    }
    if(t.nodes[nodeId].blockNodeId.empty()) {
      os << "0";
    }
    os << ")UserId(" << t.nodes[nodeId].getUserId() << ")" << endl;
  }
  os << "FreeCode = " << t.freeCodeCount() << endl;
  os << "UsedCode = " << t.usedCodeCount() << endl;
  os << "BlkCode  = " << t.blockCodeCount() << endl;
  return os;
}

///////////////////////////////////////////////////////////////////////////////

static int SHORTEST_CODE_LEN = 8;

Assigner::Assigner()
{
}

Assigner::~Assigner()
{
}

int Assigner::calcGroupCapacity(int groupNumber)
{
  assert(groupNumber >= 0 && groupNumber < 4);
  if (groupNumber < 0 || groupNumber >= 4)
    return -1;

  unsigned int level = tree.getTreeLevel();
  //cout <<"calcGroupCapacity=" << level << endl;

  // count capacity for each group
  int beginNode = 1 << level;
  int nodeCount = 1 << level;
  int nodePerGroup = nodeCount / 4;
  int capacity = 0;
  int offset = groupNumber * nodePerGroup;
  for (int k=0; k<nodePerGroup; k++) {
    int nodeId = beginNode + offset + k;
    if (tree.nodes[nodeId].isFreeCode()) {
      capacity++;
    }
  }
  return capacity;
}

int Assigner::findMinBucket(int assignCost)
{
  // count capacity for each group
  int capacity[4];
  for (int n=0; n<4; n++) {
    capacity[n] = calcGroupCapacity(n);
  }
  //for (int n=0; n<4; n++) {
  //  cout << "C[" << n << "]=" << capacity[n] << endl;
  //}

  // find the least non-zero capacity
  int bucket = -1;
  int minCapacity = 10000;
  for (int n=0; n<4; n++) {
    if (capacity[n] < assignCost)
      continue;

    if (capacity[n] < minCapacity) {
      bucket = n;
      minCapacity = capacity[n];
    }
  }
  assert(bucket == -1 || (bucket >= 0 && bucket < 4));
  return bucket;
}

std::vector<WHCode> Assigner::getWHCodeByUserId(int userId) const
{
  std::vector<WHCode> usedCode;
  for (int nodeId = 1; nodeId <= tree.codeCount(); nodeId++) {
    if (tree.nodes[nodeId].isUsedCode() && 
	tree.nodes[nodeId].getUserId() == userId) {
      usedCode.push_back(tree.nodes[nodeId].getWHCode());
    }
  }
  return usedCode;
}

bool Assigner::hasUserId(int userId) const
{
  for (int nodeId = 1; nodeId <= tree.codeCount(); nodeId++) {
    if (tree.nodes[nodeId].isUsedCode() && 
	tree.nodes[nodeId].getUserId() == userId) {
      return true;
    }
  }
  return false;
}

bool Assigner::validateRequestCodeLength(int codeLen) const
{
  return (codeLen >= SHORTEST_CODE_LEN && Math_IsPowerOfTwo(codeLen));
}

bool Assigner::validateRequestCodeLength(const std::vector<int>& codeLen) const
{
  for (size_t i=0; i<codeLen.size(); i++) {
    // all request length has to be a power of two and >= SHORTEST_CODE_LEN
    if (codeLen[i] < SHORTEST_CODE_LEN) 
      return false;

    if (!Math_IsPowerOfTwo(codeLen[i]))
      return false;
  }
  
  if (hasExceedCapacity(codeLen)) 
    return false;

  std::vector<int> tmp = codeLen;
  std::sort(tmp.begin(), tmp.end());
 
  // counter # of each code length
  std::map<int,int> counter;
  for (size_t i=0; i<tmp.size(); i++) {
    if (counter.find(tmp[i]) == counter.end()) {
      counter[tmp[i]] = 1;
    }
    else {
      counter[tmp[i]] = counter[tmp[i]] + 1;
    }
  }

  // each code length group should not have member more than group #
  std::map<int,int>::iterator it;
  for (it=counter.begin(); it != counter.end(); it++) {
    //cout << it->first << ":" << it->second << endl;
    int memberCount = it->second;
    int memberMax = it->first;
    if (memberCount > memberMax)
      return false;
  }
  
  return true;
}

std::pair<bool,WHCode> Assigner::assignUserId(int userId, int codeLens)
{
  assert(userId > 0); // userId must > 0

  if (!validateRequestCodeLength(codeLens))
    return std::make_pair(false,WHCode());

  // check if the request code length is available
  if (codeLens < calcShortestFreeCode(codeLens)) 
    return std::make_pair(false,WHCode());

  // look for the non-full bucket that has the least capacity
  unsigned int level = tree.getTreeLevel();
  unsigned int assignLevel = Math_Log2(codeLens);
  if (assignLevel > level) {
    // there is not enough nodes
    tree.expandTreeByLevel(assignLevel);
    level = tree.getTreeLevel();
  }

  assert(assignLevel <= level);
  int assignCost = 1 << (level - assignLevel);
  int bucket = findMinBucket(assignCost);
  if (bucket == -1) {
    return std::make_pair(false,WHCode());  // not enough capacity
  }
  // assign userId to that bucket
  int beginNode = 1 << assignLevel;
  int nodeCount = 1 << assignLevel;
  int nodePerGroup = nodeCount / 4;
  //cout << assignLevel << ":" << nodePerGroup << endl;

  int offset = bucket * nodePerGroup; 
  for (int k=0; k<nodePerGroup; k++) {
    int nodeId = beginNode + offset + k;
    //cout << "-> nodeId is " << nodeId << endl;
    if (tree.nodes[nodeId].isFreeCode()) {
		WHCode code;
	 
#ifdef _OVSFTREE_SANITY_CHECK_ENABLE
	  std::vector<std::pair<int,WHCode> > usedCode = listUsedCode();
	  bool status = tree.assign(assignLevel,offset+k,userId);
      assert(status);
      tree.peek(assignLevel,offset+k,code);
	  for (size_t k=0; k<usedCode.size(); k++) {
		  assert(usedCode[k].second.isOrthogonal(code));
	  }
#else
		bool status = tree.assign(assignLevel,offset+k,userId);
		assert(status);
		tree.peek(assignLevel,offset+k,code);
#endif
      return std::make_pair(true,code);
    }
  }
  // This means, eventhough this group does not have enough capacity for the current
  // capacity request.
  return std::make_pair(false,WHCode());
}

std::pair<bool,WHCode> Assigner::assignUserId(int userId, int minLen, int maxLen) 
{
  int adjustedMinLen = 1 << Math_Log2(minLen);
  int adjustedMaxLen = 1 << Math_Log2(maxLen);

  std::pair<bool,WHCode> result;
  for (int len = adjustedMinLen; len <= adjustedMaxLen; len*=2) {
    //cout << "request len = " << len << endl;
    result = assignUserId(userId,len);
    if (result.first) 
      return result;
  }
  return std::make_pair(false,WHCode());
}

std::vector<std::pair<int,WHCode> > Assigner::assignUserIds(const std::vector<int>& userId,
							    const std::vector<int>& codeLens)
{
  std::vector<std::pair<int,WHCode> > codes;
  if (userId.size() != codeLens.size())
    return codes;

  if (!validateRequestCodeLength(codeLens)) {
    return codes;
  }

  std::map<int,std::vector<int> > requests;
  for (size_t k=0; k<userId.size(); k++) {
    if (requests.find(codeLens[k]) == requests.end()) {
      std::vector<int> ids;
      ids.push_back(userId[k]);
      requests[codeLens[k]] = ids;
    }
    else {
      requests[codeLens[k]].push_back(userId[k]);
    }
  }

  // We want to assign the code from the largest code to the smallest code
  std::map<int,std::vector<int> >::reverse_iterator rit;
  for (rit = requests.rbegin(); rit != requests.rend(); ++rit) {
    std::vector<int>::iterator it;
    for (it = rit->second.begin(); it != rit->second.end(); ++it) {
      cout << "request code len of " << rit->first 
	   << " for userId of " << *it 
	   << endl;
      std::pair<bool,WHCode> code = assignUserId(*it, rit->first);
      assert(code.first); 
      codes.push_back(std::make_pair(*it,code.second));  
    }
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
  assert(Math_IsPowerOfTwo(requestLen));
  if (!Math_IsPowerOfTwo(requestLen)) 
    return 0;

  int lastNode = tree.codeCount();
  unsigned int lastLevel = Math_Log2(lastNode - 1);
  for (unsigned int lv = 2; lv <= lastLevel; lv++) {
    int codeLen = 1 << lv;

    if (codeLen < requestLen)
      continue;

    int beginNode = 1 << lv;
    int nodeCount = 1 << lv;
    
    //cout << "beginNode = " << beginNode << " endingNode=" << beginNode + nodeCount - 1 << endl;

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
  unsigned int level = Math_Log2(lastNode - 1);
  //cout << "level=" << level << endl;

  int count = 0;
  int beginNode = 1 << level;
  for (int k=beginNode; k<=lastNode; k++) {
    if (tree.nodes[k].isFreeCode()) {
      count++;
    }
  }
  //cout << "count = " << count << endl;
  //cout << "number = " << lastNode - beginNode + 1 << endl;
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
  return (sum >= 1.0);
}

///////////////////////////////////////////////////////////////////////////////

FixedLengthAssigner::FixedLengthAssigner(int codeLength, int specialUserId): 
	Assigner(),
	m_codeLength(codeLength),
	m_specialUserId(specialUserId)
{
}

FixedLengthAssigner::~FixedLengthAssigner() 
{
}

std::pair<bool,WHCode> FixedLengthAssigner::assignUserId(int userId, int codeLen)
{
	if (userId != m_specialUserId)
		return Assigner::assignUserId(userId,m_codeLength);
	else
		return Assigner::assignUserId(userId,codeLen);
}

std::pair<bool,WHCode> FixedLengthAssigner::assignUserId(int userId, int minLen, int maxLen)
{
	if (userId != m_specialUserId)
		return Assigner::assignUserId(userId,m_codeLength);
	else
		return Assigner::assignUserId(userId,minLen,maxLen);
}

std::vector<std::pair<int,WHCode> > FixedLengthAssigner::assignUserIds(	const std::vector<int>& userId, 
																		const std::vector<int>& codeLens)
{
	std::vector<int> adjustedCodeLen;
	for (size_t k=0; k<codeLens.size(); k++) {
		adjustedCodeLen.push_back(m_codeLength);
	}
	return Assigner::assignUserIds(userId,adjustedCodeLen);
}

///////////////////////////////////////////////////////////////////////////////

std::vector<WHCode> CDMA_GenerateCode(int numUsers)
{
  std::vector<WHCode> codes;

  Assigner assigner;
  int codeLen = numUsers; // Z2: this is not optimal length. The optimal length is numUsers / 2
  for (int n=1; n<=numUsers; n++) {
    std::pair<bool,WHCode> result = assigner.assignUserId(n,codeLen);
    assert(result.first);
    codes.push_back(result.second);
  }
  return codes;
}

unsigned int Math_Log2(unsigned int v)
{
  unsigned int shift = 0;
  while (v >>= 1) 
    shift++;
  return shift;
}

bool Math_IsPowerOfTwo(unsigned int v)
{
  return ((v > 0) && ((v & (v-1)) == 0));
}

int Poisson_Rand(const double lambda)
{
	double r = -1.0 * (double)lambda;
	double L = exp(r);
	int k = 0;
	double p = 1.0;
	do {
		k = k + 1;
		double u =((double)rand()/(double)RAND_MAX);
		p = p * u;
	} while (p > L);
	return k - 1;
}

///////////////////////////////////////////////////////////////////////////////




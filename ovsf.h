///////////////////////////////////////////////////////////////////////////////
// OVSF module
// - WHCode
// - OVSFTree
//
// Author: COEN233-S (Z2)
// Feb 2013
///////////////////////////////////////////////////////////////////////////////

#ifndef _OVSF_H_
#define _OVSF_H_

///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <string>

///////////////////////////////////////////////////////////////////////////////

class WHCode
{
public:
  WHCode();
  WHCode(const WHCode& base, std::vector<int> pattern);
  WHCode(const std::vector<int> initBits);
  WHCode(const WHCode& other);

  virtual ~WHCode();

  int dot(const WHCode& rhs) const;
  bool equals(const WHCode& rhs) const;
  int length() const;

  void negate();

  WHCode& operator=(const WHCode& rhs);

  std::vector<WHCode> split(int k) const;

  int getChipBit(unsigned int index) const;
  void print() const;
  std::string toHexString() const;
  std::string toByteArray() const;

protected:
  std::vector<int> bits;
};

///////////////////////////////////////////////////////////////////////////////

class OVSFTree
{
public:
  class NodeInfo {
  public:
    NodeInfo(const WHCode& wh_code, int id);
    NodeInfo(int id);
    NodeInfo();
    NodeInfo(const NodeInfo& other);

    int getUserId() const { return userId; }
    int getNodeId() const { return nodeId; }
    std::vector<int> getBlockNodeIds() const { return blockNodeId; }
    WHCode getWHCode() const { return code; }

    NodeInfo& setUserId(int id);
    NodeInfo& setNodeId(int id);
    NodeInfo& setBlockNodeId(int id);

    NodeInfo& unsetUserId();
    NodeInfo& unsetBlockNodeId(int id);
    NodeInfo& unsetAllBlockNodeId();

    NodeInfo& operator=(const NodeInfo& rhs);

    bool isUsedCode() const { return userId != 0; }
    bool isBlockCode() const { return !blockNodeId.empty(); }
    bool isFreeCode() const {
      return (!isUsedCode() && !isBlockCode());
    }

    std::vector<int> blockNodeId; // the nodeId that blocks this node
  
  private:
    // Data
    int userId; // the userId that is using this node
    int nodeId; // the nodeId of this node
    WHCode code;
  };

  OVSFTree(int initialSize = 15);
  virtual ~OVSFTree();

  bool peek(int level, int nodeId, WHCode& code) const;
  bool assign(int level, int nodeId, int userId);
  bool release(int level, int nodeId);
  bool releaseUserId(int userId);
  void releaseAll();

  bool isBlock(int level, int id) const;

  void print() const;

  int codeCount() const;
  int usedCodeCount() const;
  int freeCodeCount() const;
  int blockCodeCount() const;
 
  std::vector<std::pair<int,WHCode> > listUsedCode() const;

  int expandTreeByLevel(unsigned int level);
  int expandTree(unsigned int size);
  unsigned int getTreeLevel() const;

  // Utils
  unsigned int log2(unsigned int v) const;
  bool isPowerOfTwo(unsigned int v) const;
  int convertToNodeId(int level, int id) const;
  bool validate(int level, int id) const;
  bool isAncestorFreeCode(int nodeId) const;

  int setAncestorBlockNode(int nodeId, bool isBlock);
  int setDescendantBlockNode(int nodeId, bool isBlock);

  // data
  std::vector<NodeInfo> nodes;
};

///////////////////////////////////////////////////////////////////////////////

class Assigner 
{
public:
  Assigner();
  virtual ~Assigner();

  // Use Compact coding assignment algorithm.
  // This method will reject any codeLen <= 2
  std::pair<bool,WHCode> assignUserId(int userId, int codeLen);

  std::pair<bool,WHCode> assignUserId(int userId, int minLen, int maxLen);

  std::vector<std::pair<bool,WHCode> > assignUserIds(const std::vector<int>& userId,
						     const std::vector<int>& codeLens);

  void releaseUserId(int userId);
  void releaseAll();

  // check if the request code length is valid. It must be >= 4 and a power of two.
  bool validateRequestCodeLength(int codeLen) const;

  // check if the given request code lengths can be found on OVSF Tree.
  bool validateRequestCodeLength(const std::vector<int>& codeLen) const;

  // list all used code and its userId
  std::vector<std::pair<int, WHCode> > listUsedCode() const;

  // return all codes that are assigned to the given userId
  std::vector<WHCode> getWHCodeByUserId(int userId) const;

  // query if the given userId has assigned some WHCode
  bool hasUserId(int userId) const;

  // requestLen : target code length (must be a power of two)
  // return the code length >= requestLen
  // or zero when there is no code available
  int calcShortestFreeCode(int requestLen) const;

  // query the current capacity of the OVSF Tree. The capacity is 
  // a fraction between [0,1.0]. 1.0 means 100% free capacity. 
  double calcCurrentCapacity() const;

  void print() const;

  // check if the given requested code length is valid. Meaning,
  // there exists some configuration that satisfy this capacity requirement.
  static bool hasExceedCapacity(const std::vector<int>& codeLength);

protected:
  int findMinBucket(int assignCost);
  int calcGroupCapacity(int groupNumber);

  OVSFTree tree;
};

///////////////////////////////////////////////////////////////////////////////

// generate a set of nunUsers CDMA chip sequence of length numUsers bits
// Z2: The CDMA generator algorithm is using OVSFTree ... so the 
// chip sequence len is not optimal.
std::vector<WHCode> CDMA_GenerateCode(int numUsers);

///////////////////////////////////////////////////////////////////////////////

#endif // _OVSF_H_

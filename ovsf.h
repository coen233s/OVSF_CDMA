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
#include <fstream>

///////////////////////////////////////////////////////////////////////////////

class WHCode
{
public:
  friend std::ostream& operator<<(std::ostream& os, const WHCode& wc);

  WHCode();
  WHCode(const std::string byteArray);
  WHCode(const WHCode& base, std::vector<int> pattern);
  WHCode(const std::vector<int> initBits);
  WHCode(const WHCode& other);

  virtual ~WHCode();

  int dot(const WHCode& rhs) const;
  bool isOrthogonal(const WHCode& rhs) const;

  bool equals(const WHCode& rhs) const;
  int length() const;

  void negate();

  WHCode& operator=(const WHCode& rhs);
  bool operator==(const WHCode& rhs);

  std::vector<WHCode> split(int k) const;

  int getChipBit(unsigned int index) const;
  void print() const;
  std::string toHexString() const;
  std::string toByteArray() const;

protected:
  std::vector<int> bits;
};

std::ostream& operator<<(std::ostream& os, const WHCode& wc);

///////////////////////////////////////////////////////////////////////////////

class OVSFTree
{
public:
  friend std::ostream& operator<<(std::ostream& os, const OVSFTree& tree);

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
  int convertToNodeId(int level, int id) const;
  bool validate(int level, int id) const;
  bool isAncestorFreeCode(int nodeId) const;

  int setAncestorBlockNode(int nodeId, bool isBlock);
  int setDescendantBlockNode(int nodeId, bool isBlock);

  // data
  std::vector<NodeInfo> nodes;
};

std::ostream& operator<<(std::ostream& os, const OVSFTree& tree);

///////////////////////////////////////////////////////////////////////////////

class AssignerInterface
{
public:
	AssignerInterface() {}
	virtual ~AssignerInterface() {}

  virtual std::pair<bool,WHCode> assignUserId(int userId, int codeLen) = 0;
  virtual std::pair<bool,WHCode> assignUserId(int userId, int minLen, int maxLen) = 0;
  virtual std::vector<std::pair<int,WHCode> > assignUserIds(const std::vector<int>& userId,
													const std::vector<int>& codeLens) = 0;

  virtual void releaseUserId(int userId) = 0;
  virtual void releaseAll() = 0;

  // check if the request code length is valid. It must be >= 4 and a power of two.
  virtual bool validateRequestCodeLength(int codeLen) const = 0;

  // check if the given request code lengths can be found on OVSF Tree.
  virtual bool validateRequestCodeLength(const std::vector<int>& codeLen) const = 0;

  // list all used code and its userId
  virtual std::vector<std::pair<int, WHCode> > listUsedCode() const = 0;

  // return all codes that are assigned to the given userId
  virtual std::vector<WHCode> getWHCodeByUserId(int userId) const = 0;

  // query if the given userId has assigned some WHCode
  virtual bool hasUserId(int userId) const = 0;

  // requestLen : target code length (must be a power of two)
  // return the code length >= requestLen
  // or zero when there is no code available
  virtual int calcShortestFreeCode(int requestLen) const = 0;

  // query the current capacity of the OVSF Tree. The capacity is 
  // a fraction between [0,1.0]. 1.0 means 100% free capacity. 
  virtual double calcCurrentCapacity() const = 0;

  virtual void print() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Dynamic Length CDMA code assigner
class Assigner: public AssignerInterface
{
public:
  Assigner();
  virtual ~Assigner();

  // Use Compact coding assignment algorithm.
  // This method will reject any codeLen <= 2
  virtual std::pair<bool,WHCode> assignUserId(int userId, int codeLen);

  virtual std::pair<bool,WHCode> assignUserId(int userId, int minLen, int maxLen);

  virtual std::vector<std::pair<int,WHCode> > assignUserIds(const std::vector<int>& userId,
						    const std::vector<int>& codeLens);

  virtual void releaseUserId(int userId);
  virtual void releaseAll();

  // check if the request code length is valid. It must be >= 4 and a power of two.
  virtual bool validateRequestCodeLength(int codeLen) const;

  // check if the given request code lengths can be found on OVSF Tree.
  virtual bool validateRequestCodeLength(const std::vector<int>& codeLen) const;

  // list all used code and its userId
  virtual std::vector<std::pair<int, WHCode> > listUsedCode() const;

  // return all codes that are assigned to the given userId
  virtual std::vector<WHCode> getWHCodeByUserId(int userId) const;

  // query if the given userId has assigned some WHCode
  virtual bool hasUserId(int userId) const;

  // requestLen : target code length (must be a power of two)
  // return the code length >= requestLen
  // or zero when there is no code available
  virtual int calcShortestFreeCode(int requestLen) const;

  // query the current capacity of the OVSF Tree. The capacity is 
  // a fraction between [0,1.0]. 1.0 means 100% free capacity. 
  virtual double calcCurrentCapacity() const;

  virtual void print() const;

  // check if the given requested code length is valid. Meaning,
  // there exists some configuration that satisfy this capacity requirement.
  static bool hasExceedCapacity(const std::vector<int>& codeLength);
  
protected:
  int findMinBucket(int assignCost);
  int calcGroupCapacity(int groupNumber);

  OVSFTree tree;
};

///////////////////////////////////////////////////////////////////////////////
// Fixed-length CDMA code assigner
class FixedLengthAssigner: public Assigner
{
public:
	FixedLengthAssigner(int codeLength, int specialUserId = 1);
	virtual ~FixedLengthAssigner();

  virtual std::pair<bool,WHCode> assignUserId(int userId, int codeLen);
  virtual std::pair<bool,WHCode> assignUserId(int userId, int minLen, int maxLen);
  virtual std::vector<std::pair<int,WHCode> > assignUserIds(const std::vector<int>& userId,
													const std::vector<int>& codeLens);

protected:
	int m_codeLength;
	int m_specialUserId;
};

///////////////////////////////////////////////////////////////////////////////

// generate a set of nunUsers CDMA chip sequence of length numUsers bits
// Z2: The CDMA generator algorithm is using OVSFTree ... so the 
// chip sequence len is not optimal.
std::vector<WHCode> CDMA_GenerateCode(int numUsers);

// Math utils
unsigned int Math_Log2(unsigned int v);
bool Math_IsPowerOfTwo(unsigned int v);

///////////////////////////////////////////////////////////////////////////////

#endif // _OVSF_H_

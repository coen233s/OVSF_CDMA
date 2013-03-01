///////////////////////////////////////////////////////////////////////////////
// OVSF module
// - WHCode
// - OVSFTree
//
// Author: COEN233-S
// Feb 2013
///////////////////////////////////////////////////////////////////////////////

#ifndef _OVSF_H_
#define _OVSF_H_

///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <queue>
#include <map>
#include <algorithm>

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

protected:
  std::vector<int> bits;
};

///////////////////////////////////////////////////////////////////////////////

class OVSFTree
{
public:
  class NodeInfo {
  public:
    NodeInfo(int id);
    NodeInfo();
    NodeInfo(const NodeInfo& other);

    NodeInfo& setUserId(int id);
    NodeInfo& setNodeId(int id);
    NodeInfo& setBlockNodeId(int id);

    NodeInfo& unsetUserId();
    NodeInfo& unsetBlockNodeId(int id);

    NodeInfo& operator=(const NodeInfo& rhs);

    int userId; // the userId that is using this node
    int nodeId; // the nodeId of this node
    std::vector<int> blockNodeId; // the nodeId that blocks this node

    bool isUsedCode() const { return userId != 0; }
    bool isBlockCode() const { return !blockNodeId.empty(); }
  };

  OVSFTree();
  virtual ~OVSFTree();

  bool peek(int level, int nodeId, WHCode& code) const;
  bool assign(int level, int nodeId, int userId);
  bool release(int level, int nodeId);
  bool isBlock(int level, int id) const;

  void print() const;

  int codeCount() const;
  int usedCodeCount() const;
  int freeCodeCount() const;
  int blockCodeCount() const;

protected:
  int expandTree(unsigned int size);
  NodeInfo getNodeInfo(int nodeId) const;
  void setNodeInfo(const int nodeId, const NodeInfo& info);

private:
  unsigned int log2(unsigned int v) const;
  int convertToNodeId(int level, int id) const;
  bool validate(int level, int id) const;
  bool isAncestorFreeCode(int nodeId) const;

  // data
  typedef std::map<int,NodeInfo> Table_T;
  Table_T nodeInfo;
  std::vector<WHCode> codes;
};

///////////////////////////////////////////////////////////////////////////////

#endif // _OVSF_H_

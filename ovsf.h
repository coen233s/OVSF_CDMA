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

    // Data
    int userId; // the userId that is using this node
    int nodeId; // the nodeId of this node
    std::vector<int> blockNodeId; // the nodeId that blocks this node
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

protected:
  int expandTree(unsigned int size);

private:
  unsigned int log2(unsigned int v) const;
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

  std::pair<bool,WHCode> assignUserId(int userId, int codeLen);
  std::vector<std::pair<bool,WHCode> > assignUserIds(std::vector<std::pair<int,int> > requestInfo);

  void releaseUserId(int userId);
  void releaseAll();

  std::vector<std::pair<int, WHCode> > listUsedCode() const;

protected:
  OVSFTree tree;

};

///////////////////////////////////////////////////////////////////////////////

#endif // _OVSF_H_

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

  int getChipBit(size_t index) const;
  void print() const;

protected:
  std::vector<int> bits;
};

///////////////////////////////////////////////////////////////////////////////

class OVSFTree
{
public:
  struct NodeInfo {
    int userId; // the userId that is using this node
    int nodeId; // the nodeId of this node
    int blockNodeId; // the nodeId that blocks this node
  };

  OVSFTree();
  virtual ~OVSFTree();

  bool peek(int level, int nodeId, WHCode& code) const;
  bool assign(int level, int nodeId, int userId);
  bool release(int level, int nodeId);

  void print() const;

  int codeCount() const;
  int usedCodeCount() const;
  int freeCodeCount() const;
  int blockCodeCount() const;

  //protected:
  int expandTree(unsigned int size);

protected:
  NodeInfo getNodeInfo(int nodeId);

private:
  unsigned int log2(unsigned int v) const;

  typedef std::map<int,NodeInfo> Table_T;
  Table_T info;
  std::vector<WHCode> codes;
};

///////////////////////////////////////////////////////////////////////////////

#endif // _OVSF_H_

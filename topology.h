#ifndef _topology
#define _topology

#include <deque>
#include <string>

#include "node.h"
#include "link.h"

class Topology
{
 protected:
  deque<Node*> nodes;
  deque<Link*> links;

  deque<Node*>::iterator FindMatchingNodeIt(const Node *n);
  deque<Link*>::iterator FindMatchingLinkIt(const Link *l);
  
 public:
  Topology();
  virtual ~Topology();

  Node *FindMatchingNode(const Node *n);
  deque<Link*> *GetOutgoingLinks(const Node *n);
  deque<Node*> *GetNeighbors(const Node *n);

  void CollectShortestPathTreeLinks(const Node &src, deque<Link> &links);
  void CollectShortestPathLinks(const Node &src, const Node &dest, deque<Link> &links);

  void AddNode(Node *n);
  void DeleteNode(const Node *n);
  void ChangeNode(const Node *n);

  Link *FindMatchingLink(const Link *l);

  void AddLink(Link *n);
  void DeleteLink(const Link *n);
  void ChangeLink(const Link *n);

  void WriteDot(const string &fn) const;
  void DrawTopology() const;

  ostream & Print(ostream &os) const;
}  ;


inline ostream & operator<<(ostream &os, const Topology &t) { return t.Print(os);};


#endif


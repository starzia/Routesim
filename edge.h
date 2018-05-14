#ifndef _node
#define _node

#include <string>
#include <iostream>

#include "graph.h"

class Node {
 private:
  string   name;
  Graph    *graph;

 public:
  Node(const string n, Graph *g);
  Node(const Node &node);
  Node & operator=(const Node &rhs);
  virtual ~Node();

  ostream & Print(ostream &os) const;

}

inline ostream & operator<<(ostream &os, const Node &n) { return n.Print(os);}


#endif

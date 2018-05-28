#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include "context.h"



SimulationContext::SimulationContext()
{}

SimulationContext::~SimulationContext()
{}

void SimulationContext::LoadEvents(const string &file)
{
  
  FILE *in = fopen(file.c_str(),"r");
  if (in==0) { 
    cerr << "Can't read events from "<<file<<endl;
    exit(-1);
  }
  while (!feof(in)) {
    char b[1024];
    char *buf=b;
    char cmd[1024];
    unsigned num;
    unsigned src,dest;
    double lat, bw;
    double timestamp;
    if (!fgets(buf,1024,in)) {
      break;
    }
    if (strlen(buf)==0){
	continue;
    }
    while (isspace(*buf)) { 
      buf++;
    }
    if (*buf==0) {
      continue;
    }
    if (toupper(buf[0])=='#') {
      continue;
    }
    //fprintf(stderr,"%s",buf);
    sscanf(buf,"%lf %s",&timestamp,cmd);
    if (!strcasecmp(cmd,"DRAW_TOPOLOGY")) {
      sscanf(buf,"%lf %s",&timestamp,cmd);
      PostEvent(new Event(timestamp,DRAW_TOPOLOGY,this,0));
      continue;
    }
    if (!strcasecmp(cmd,"DUMP_TABLE")) {
      sscanf(buf,"%lf %s %u",&timestamp,cmd,&num);
      PostEvent(new Event(timestamp,DUMP_TABLE,this,new Node(num,this,0,0)));
      continue;
    }
    if (!strcasecmp(cmd,"DRAW_TREE")) {
      sscanf(buf,"%lf %s %u",&timestamp,cmd,&num);
      PostEvent(new Event(timestamp,DRAW_TREE,this,new Node(num,this,0,0)));
      continue;
    }
    if (!strcasecmp(cmd,"DRAW_PATH")) {
      sscanf(buf,"%lf %s %u %u",&timestamp,cmd,&src,&dest);
      PostEvent(new Event(timestamp,DRAW_PATH,this,new Link(src,dest,this,0,0)));
      continue;
    }
#if 0
    if (!strcasecmp(cmd,"WRITE_TOPOLOGY")) {
      char file[1024];
      sscanf(buf,"%lf %s %s",&timestamp,cmd,file);
      PostEvent(new Event(timestamp,WRITE_TOPOLOGY,this,new string(file)));
      continue;
    }
    if (!strcasecmp(cmd,"WRITE_TREE")) {
      char file[1024];
      sscanf(buf,"%lf %s %s",&timestamp,cmd,file);
      PostEvent(new Event(timestamp,WRITE_TREE,this,new string(file)));
      continue;
    }
#endif
    if (!strcasecmp(cmd,"ADD_NODE")) {
      sscanf(buf,"%lf %s %u %lf %lf",&timestamp,cmd,&num,&lat,&bw);
      PostEvent(new Event(timestamp,ADD_NODE,this,new Node(num,this,bw,lat)));
      continue;
    }
    if (!strcasecmp(cmd,"DELETE_NODE")) {
      sscanf(buf,"%lf %s %u %lf %lf",&timestamp,cmd,&num,&lat,&bw);
      PostEvent(new Event(timestamp,DELETE_NODE,this,new Node(num,this,bw,lat)));
      continue;
    }
    if (!strcasecmp(cmd,"CHANGE_NODE")) {
      sscanf(buf,"%lf %s %u %lf %lf",&timestamp,cmd,&num,&lat,&bw);
      PostEvent(new Event(timestamp,CHANGE_NODE,this,new Node(num,this,bw,lat)));
      continue;
    }
    if (!strcasecmp(cmd,"ADD_LINK")) {
      sscanf(buf,"%lf %s %u %u %lf %lf",&timestamp,cmd,&src,&dest,&lat,&bw);
      PostEvent(new Event(timestamp,ADD_LINK,this,new Link(src,dest,this,bw,lat)));
      continue;
    }
    if (!strcasecmp(cmd,"DELETE_LINK")) {
      sscanf(buf,"%lf %s %u %u %lf %lf",&timestamp,cmd,&src,&dest,&lat,&bw);
      PostEvent(new Event(timestamp,DELETE_LINK,this,new Link(src,dest,this,bw,lat)));
      continue;
    }
    if (!strcasecmp(cmd,"CHANGE_LINK")) {
      sscanf(buf,"%lf %s %u %u %lf %lf",&timestamp,cmd,&src,&dest,&lat,&bw);
      PostEvent(new Event(timestamp,CHANGE_LINK,this,new Link(src,dest,this,bw,lat)));
      continue;
    }
    if (!strcasecmp(cmd,"PRINT")) {
      sscanf(buf,"%lf %s",&timestamp,cmd);
      char *start;
      char *data = new char [1024];
      start=strstr(buf,"PRINT")+5;
      while (*start!=0 && isspace(*start)) { 
	start++;
      }
      if (*start==0) { 
	strncpy(data,"Nothing to print!",1024);
      } else {
	strncpy(data,start,1024);
      }
      PostEvent(new Event(timestamp,PRINT,this,data));
      continue;
    }
  }
  fclose(in);
}

void SimulationContext::Init()
{
    for (deque<Link*>::const_iterator i=links.begin();i!=links.end();++i) { 
	//PostEvent(new Event(0x80000000, CHANGE_LINK,this,new Link(**i)));
	PostEvent(new Event(-100, CHANGE_LINK,this,new Link(**i)));
    }
}

void SimulationContext::LoadTopology(const string &file)
{
  LoadEvents(file);
  Event *e;
  while ((e=GetEarliestEvent())) { 
    e->Dispatch();
    e->Disassociate();
    delete e;
  }
}

ostream & SimulationContext::Print(ostream &os)
{
  os << "SimulationContext(topology=";
  Topology::Print(os);
  os <<", eventqueue=";
  EventQueue::Print(os);
  os<<")";
  return os;
}



void SimulationContext::DrawShortestPathTree(const Node *node) const
{
  WriteShortestPathTreeDot(node,"_tree.in");
  system("dot _tree.in > _tree.out");
  system("dotty _tree.out");
}

void SimulationContext::DispatchEvent(Event *e)
{
  e->Dispatch();
  e->Disassociate();
  delete e;
}


// This is just used by the map container to build a search tree.
// It defines a consistent "total ordering" of the links.
struct link_compare {
  bool operator() ( const Link &l, const Link &r) const {
    return (l.GetSrc()<r.GetSrc()) || (l.GetSrc()==r.GetSrc() && l.GetDest()<r.GetDest());
  }
};


void SimulationContext::WriteShortestPathTreeDot(const Node *src, const string &s) const
{
  cerr << "********BEGIN WRITE TREE"<<endl;
  FILE *out = fopen(s.c_str(),"w");
  if (out==0) { 
    return;
  } 
  // Yes, this is hideously slow
  map<Link, int, link_compare> treelinks;
  deque<Link> path;
  for (deque<Node*>::const_iterator i=nodes.begin();i!=nodes.end();++i) { 
    path.clear();
    CollectPathLinks(*src,**i,path);
    for (deque<Link>::const_iterator j=path.begin();j!=path.end();++j) {
      treelinks[*j]=1;
    }
  }
  deque<Link> realtree;
  ((SimulationContext*)this)->CollectShortestPathTreeLinks(*src,realtree);

  fprintf(out,"digraph tree {\n");
  for (deque<Node*>::const_iterator i=nodes.begin(); i!=nodes.end();++i) {
    fprintf(out,"%u\n",(*i)->GetNumber());
  }
  for (deque<Link*>::const_iterator i=links.begin(); i!=links.end();++i) {
    fprintf(out,"%u -> %u [ label=\"%5.1lf\" ];\n",(*i)->GetSrc(),(*i)->GetDest(), (*i)->GetLatency());
  }
  for (deque<Link>::const_iterator i=realtree.begin(); i!=realtree.end();++i) {
    fprintf(out,"%u -> %u [ color=blue ];\n",(*i).GetSrc(),(*i).GetDest());
  }
  for (map<Link,int,link_compare>::const_iterator i=treelinks.begin();i!=treelinks.end();++i) {
    Link l = (*i).first;
    fprintf(out,"%u -> %u [ color=red ];\n",l.GetSrc(),l.GetDest());
    bool found=false;
    for (deque<Link>::const_iterator j=realtree.begin(); j!=realtree.end();++j) { 
      if ((*j).GetSrc()==l.GetSrc() && (*j).GetDest()==l.GetDest()) { 
	found=true;
	break;
      }
    }
    if (!found) { 
      cout << "SUSPICIOUS: "<<l.GetSrc()<<" -> "<<l.GetDest()<<" not found in actual shortest paths tree"<<endl;
    }
  }
  fprintf(out,"}\n");
  fclose(out);
  cerr << "********END WRITE TREE"<<endl;
}

void SimulationContext::WritePathDot(const Node &src, const Node &dest, const string &s) const
{
  cerr << "********BEGIN WRITE PATH"<<endl;
  FILE *out = fopen(s.c_str(),"w");
  if (out==0) { 
    return;
  } 
  deque<Link> path;
  CollectPathLinks(src,dest,path);
  deque<Link> realpath;
  ((SimulationContext*)this)->CollectShortestPathLinks(src,dest,realpath);
  fprintf(out,"digraph path {\n");
  for (deque<Node*>::const_iterator i=nodes.begin(); i!=nodes.end();++i) {
    fprintf(out,"%u\n",(*i)->GetNumber());
  }
  for (deque<Link*>::const_iterator i=links.begin(); i!=links.end();++i) {
    fprintf(out,"%u -> %u [ label=\"%5.1lf\" ];\n",(*i)->GetSrc(),(*i)->GetDest(), (*i)->GetLatency());
  }
  for (deque<Link>::const_iterator i=realpath.begin();i!=realpath.end();++i) {
    fprintf(out,"%u -> %u [ color=blue ];\n",(*i).GetSrc(),(*i).GetDest());
  }
  for (deque<Link>::const_iterator i=path.begin();i!=path.end();++i) {
    fprintf(out,"%u -> %u [ color=red ];\n",(*i).GetSrc(),(*i).GetDest());
    Link l =(*i);
    bool found=false;
    for (deque<Link>::const_iterator j=realpath.begin(); j!=realpath.end();++j) { 
      if ((*j).GetSrc()==l.GetSrc() && (*j).GetDest()==l.GetDest()) { 
	found=true;
	break;
      }
    }
    if (!found) { 
      cout << "SUSPICIOUS: "<<l.GetSrc()<<" -> "<<l.GetDest()<<" not found in actual shortest paths tree"<<endl;
    }
  }
  fprintf(out,"}\n");
  fclose(out);
  cerr << "********END WRITE PATH"<<endl;
}

void SimulationContext::DrawPath(const Link *p) const
{
  WritePathDot(Node(p->GetSrc(),0,0,0),Node(p->GetDest(),0,0,0),string("_path.in"));
  system("dot _path.in > _path.out");
  system("dotty _path.out");
}


void SimulationContext::CollectPathLinks(const Node &src, const Node &dest, deque<Link> &path) const
{
  Node *n=((SimulationContext*)this)->FindMatchingNode(&src);
  if (n==0) { 
    return;
  }
  unsigned last=n->GetNumber();
  unsigned count=0;
  while (n->GetNumber()!=dest.GetNumber()) {
    Node *next_node=n->GetNextHop(&dest);
    if (next_node==0) {
      break;
    }
    n=((SimulationContext *)this)->FindMatchingNode(next_node);
    if (n==0) {
      delete next_node;
      break;
    }
    //    cerr << last <<" -> " << n->GetNumber()<<endl;
    path.push_back(Link(last,n->GetNumber(),0,0,0));
    last=n->GetNumber();
    delete next_node;
    count++;
    if (count>(nodes.size()*2)) {
      cerr << "SimulationContext::CollectPathLinks terminating prematurely due to suspected routing loop!\n";
      break;
    }
  }
 
}


void SimulationContext::DumpTable(const Node *src)
{
  cout <<*(FindMatchingNode(src))<< endl;
}


void SimulationContext::TimeOut(const Node *src, const double timefromnow)
{
  PostEvent(new Event(GetTime()+timefromnow,
		      TIMEOUT,
		      FindMatchingNode(src),
		      0));
}
		      

void SimulationContext::SendToNeighbors(const Node *src, const RoutingMessage *m)
{
  deque<Link*> *ll=GetOutgoingLinks(src);
  for (deque<Link*>::const_iterator i=ll->begin();i!=ll->end();++i) {
    Node x = Node((*i)->GetDest(),0,0,0);
    PostEvent(new Event(GetTime()+(*i)->GetLatency(),
			ROUTING_MESSAGE_ARRIVAL,
			FindMatchingNode(&x),
			(void*)m));
  }
  delete ll;
}

void SimulationContext::SendToNeighbor(const Node *src, const Node *dest, const RoutingMessage *m)
{
  Link x = Link(src->GetNumber(),dest->GetNumber(),0,0,0);
  Link *l = FindMatchingLink(&x);

  PostEvent(new Event(GetTime()+l->GetLatency(),
		      ROUTING_MESSAGE_ARRIVAL,
		      FindMatchingNode(dest),
		      (void*)m));
}
					 

#include "link.h"

Link::Link(const unsigned s, const unsigned d, SimulationContext *c, double b, double l) :
  src(s), dest(d), context(c), bw(b), lat(l) {}

Link::Link()
{}


Link::Link(const Link &rhs) :
  src(rhs.src), dest(rhs.dest), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

Link & Link::operator=(const Link &rhs)
{
  return *(new (this) Link(rhs));
}

Link::~Link()
{}

bool Link::Matches(const Link &rhs) const
{
  return src==rhs.src && dest==rhs.dest;
}

void Link::SetSrc(const unsigned s) 
{ src=s;}

unsigned Link::GetSrc() const 
{ return src;}

void Link::SetDest(const unsigned d) 
{ dest=d;}

unsigned Link::GetDest() const 
{ return dest;}

void Link::SetLatency(const double l)
{ lat=l;}

double Link::GetLatency() const 
{ return lat; }

void Link::SetBW(const double b)
{ bw=b;}

double Link::GetBW() const
{ return bw;}

ostream & Link::Print(ostream &os) const
{
  os << "Link(src="<<src<<", dest="<<dest<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}


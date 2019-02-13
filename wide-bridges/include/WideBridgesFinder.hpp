#ifndef __WIDE_BRIDGES_FINDER
#define __WIDE_BRIDGES_FINDER

#include "Graph.hpp"
#include <iostream>

class WideBridgesFinder
{
  public:
    WideBridgesFinder();
    void find(Graph &G);
    void print(std::ostream &os, Graph &G);
};

#endif
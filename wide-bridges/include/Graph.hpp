#ifndef __GRAPH
#define __GRAPH

#include <vector>
#include <iostream>

typedef enum edge_state
{
    NO_EDGE,
    EDGE,
    CHECKED,
    WIDE_BRIDGE
} edge_state;

class WideBridgesFinder;

class Graph
{
    friend WideBridgesFinder;
    friend std::istream &operator>>(std::istream &is, Graph &G);

    int numberOfVertices;
    bool *visitedArr;
    int dfsCounter;
    edge_state **adjMatrix;
    void createAdjMatrix();
    void zeroVisitedArr();
    void dfs(int v);

  public:
    Graph();
    ~Graph();
    bool checkCohesion(int inactiveVertex1, int inactiveVertex2);
};

#endif
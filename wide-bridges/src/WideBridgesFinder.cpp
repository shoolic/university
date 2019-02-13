#include <WideBridgesFinder.hpp>

WideBridgesFinder::WideBridgesFinder(){};

void WideBridgesFinder::find(Graph &G)
{
    for (int i = 0; i < G.numberOfVertices; i++)
    {
        for (int j = 0; j < G.numberOfVertices; j++)
        {
            if (G.adjMatrix[i][j] == EDGE)
            {
                G.adjMatrix[j][i] = CHECKED;
                if (!G.checkCohesion(i, j))
                {
                    G.adjMatrix[i][j] = WIDE_BRIDGE;
                }
            }
        }
    }
}

void WideBridgesFinder::print(std::ostream &os, Graph &G)
{
    for (int i = 0; i < G.numberOfVertices; i++)
    {
        for (int j = 0; j < G.numberOfVertices; j++)
        {
            if (G.adjMatrix[i][j] == WIDE_BRIDGE)
            {
                os << i << " " << j << std::endl;
            }
        }
    }
}

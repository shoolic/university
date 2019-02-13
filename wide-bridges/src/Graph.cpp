#include <Graph.hpp>

Graph::Graph() {}

Graph::~Graph()
{
    for (int i = 0; i < numberOfVertices; i++)
    {
        delete[] adjMatrix[i];
    }
    delete[] adjMatrix;
    delete[] visitedArr;
}

bool Graph::checkCohesion(int inactiveVertex1, int inactiveVertex2)
{
    zeroVisitedArr();
    dfsCounter = 0;
    visitedArr[inactiveVertex1] = true;
    visitedArr[inactiveVertex2] = true;
    for (int i = 0; i < numberOfVertices; i++)
    {
        if (!visitedArr[i])
        {
            dfs(i);
            break;
        }
    }

    if (dfsCounter == numberOfVertices - 2)
    {
        return true;
    }

    return false;
}

void Graph::dfs(int v)
{
    visitedArr[v] = true;
    dfsCounter++;
    for (int u = 0; u < numberOfVertices; u++)
    {
        if (!visitedArr[u] && adjMatrix[v][u] > 0)
        {
            dfs(u);
        }
    }
}

void Graph::zeroVisitedArr()
{
    for (int i = 0; i < numberOfVertices; i++)
    {
        visitedArr[i] = 0;
    }
}

void Graph::createAdjMatrix()
{
    adjMatrix = new edge_state *[numberOfVertices];
    for (int i = 0; i < numberOfVertices; i++)
    {
        adjMatrix[i] = new edge_state[numberOfVertices]();
    }
}

std::istream &operator>>(std::istream &is, Graph &G)
{
    is >> G.numberOfVertices;
    G.createAdjMatrix();
    G.visitedArr = new bool[G.numberOfVertices];

    int tmp1, tmp2;
    while (is >> tmp1 >> tmp2)
    {
        G.adjMatrix[tmp1][tmp2] = EDGE;
        G.adjMatrix[tmp2][tmp1] = EDGE;
    }
}

#include <iostream>
#include <Graph.hpp>
#include <WideBridgesFinder.hpp>

int main()
{
    Graph G{};
    std::cin >> G;
    WideBridgesFinder WBF{};
    WBF.find(G);
    WBF.print(std::cout, G);
}
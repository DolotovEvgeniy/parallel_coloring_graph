#ifndef GRAPH_H
#define GRAPH_H
#include <vector>

class Graph
{
public:
    std::vector< std::vector<char> > data;
    int vertexCount;
    std::vector<int> colors;
    Graph(int vertexCount,int seed);
    void clearColors();

    void printColors();
    void printGraph();

    void findNeighbors(int vertex, std::vector<int>& neighbors);
    int smallestNeighborsColor(int vertex);

    void checkColors();

    int colorsCount();
};

#endif // GRAPH_H

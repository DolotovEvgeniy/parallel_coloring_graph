#include "graph.h"
#include <stdlib.h>
#include <iostream>
#include <algorithm>

using namespace std;
Graph::Graph(int _vertexCount, int _seed)
{
    vertexCount=_vertexCount;

    data.resize(vertexCount);
    for(int i=0;i<vertexCount;i++)
    {
        data[i].resize(vertexCount);
    }
    srand(_seed);
    int countEdge=0;
    for(int i=0;i<vertexCount;i++)
        for(int j=0;j<i;j++)
        {
            countEdge+=data[i][j]=data[j][i]=(rand()%5==0);
        }
    colors.resize(vertexCount, -1);
    clearColors();
}

void Graph::clearColors()
{
    for(int i=0;i<vertexCount;i++)
    {
        colors[i]=-1;
    }
}

void Graph::printColors()
{
    cout<<"----COLORS----"<<endl;
    for(int i=0;i<vertexCount;i++)
    {
        cout<<"Vertex "<<i<<" color: "<<colors[i]<<endl;
    }
    cout<<"--------------"<<endl;
}

void Graph::printGraph()
{
    cout<<"-----GRAPH-----"<<endl;
    for(int i=0;i<vertexCount;i++)
    {
        for(int j=0; j<vertexCount; j++)
        {
            cout<<(int)data[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"---------------"<<endl;
}

void Graph::findNeighbors(int vertex, vector<int> &neighbors)
{

    for(int i=0;i<vertexCount;i++)
    {
        if(data[vertex][i]==1)
        {
            neighbors.push_back(i);
        }
    }
}

int Graph::smallestNeighborsColor(int vertex)
{
    vector<int> neighbors;
    findNeighbors(vertex, neighbors);

    if(neighbors.empty())
    {
        return 1;
    }
    else
    {
        vector<int> neighborsColors;
        for(unsigned int i=0;i<neighbors.size();i++)
        {
            neighborsColors.push_back(colors[neighbors[i]]);
        }
        int maxColor=*std::max_element(neighborsColors.begin(), neighborsColors.end());

        if(maxColor==-1)
        {
            return 1;
        }
        else
        {

            for(int i=1;i<maxColor;i++)
            {
                if(find(neighborsColors.begin(), neighborsColors.end(), i)==neighborsColors.end())
                {
                    return i;
                }
            }
            return maxColor+1;
        }
    }
}

void Graph::checkColors()
{
    bool isCorrect=true;
    for(int i=0;i<vertexCount;i++)
    {
        vector<int> neighbors;
        findNeighbors(i, neighbors);
        vector<int> neighborsColors;
        for(unsigned int j=0;j<neighbors.size();j++)
        {
            neighborsColors.push_back(colors[neighbors[j]]);
        }

        if(find(neighborsColors.begin(), neighborsColors.end(), colors[i])!=neighborsColors.end())
        {
            isCorrect=false;
            break;
        }

    }
    if(isCorrect==true)
    {
        cout<<"Coloring graph is correct!"<<endl;
    }
    else
    {
        cout<<"Coloring graph is NOT correct!"<<endl;
    }
}

int Graph::colorsCount()
{
    return *max_element(colors.begin(), colors.end());
}

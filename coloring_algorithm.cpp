#include "coloring_algorithm.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <mpi.h>
using namespace std;
void greedyColoring(Graph& graph)
{
    for(int i=0;i<graph.vertexCount;i++)
    {
        graph.colors[i]=graph.smallestNeighborsColor(i);
    }
}

void jonesPlassmansColoring(Graph &graph, int seed)
{
    vector<int> randomNumber;
    srand(seed);
    for(int i=0;i<graph.vertexCount;i++)
    {
        randomNumber.push_back(rand());
    }

    vector<int> remainVertex;
    for(int i=0;i<graph.vertexCount;i++)
    {
        remainVertex.push_back(i);
    }

    while(!remainVertex.empty())
    {
        vector<int> newRemainVertex;
        for(unsigned int i=0; i<remainVertex.size();i++)
        {
            vector<int> neighbors;

            graph.findNeighbors(remainVertex[i], neighbors);
            vector<int> unColoredNeighborsNumber;
            for(unsigned int j=0;j<neighbors.size();j++)
            {
                if(graph.colors[neighbors[j]]==-1)
                {
                    unColoredNeighborsNumber.push_back(randomNumber[neighbors[j]]);
                }
            }

            if(unColoredNeighborsNumber.empty())
            {
                graph.colors[remainVertex[i]]=graph.smallestNeighborsColor(remainVertex[i]);
            }
            else
            {
                int maxNeighborNumber=*std::max_element(unColoredNeighborsNumber.begin(), unColoredNeighborsNumber.end());


                int indexMaxNeighbor=0;
                for(unsigned int j=0;j<neighbors.size();j++)
                {
                    if(randomNumber[neighbors[j]]==maxNeighborNumber && graph.colors[neighbors[j]]!=-1)
                    {
                        indexMaxNeighbor=j;
                    }
                }
                if(maxNeighborNumber<randomNumber[remainVertex[i]] || (maxNeighborNumber==randomNumber[remainVertex[i]] && indexMaxNeighbor<remainVertex[i]))
                {
                    graph.colors[remainVertex[i]]=graph.smallestNeighborsColor(remainVertex[i]);
                }
                else
                {
                    newRemainVertex.push_back(remainVertex[i]);
                }
            }
        }
        remainVertex=newRemainVertex;
    }
}
void sendVertexNumberToProcess(int vertexCount, int ProcNum, int& activeProc)
{
    vector<int> vertex;
    for(int i=0;i<vertexCount;i++)
    {
        vertex.push_back(i);
    }

    int batchSize=vertex.size()/(ProcNum-1);
    if(batchSize==0)
    {
        activeProc=1;
    }
    else
    {
        activeProc=ProcNum-1;
    }

    for(int i=1; i<activeProc; i++)
    {
        int begin=(i-1)*batchSize;
        int end=i*batchSize;
        MPI_Send(&begin, 1 , MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Send(&end, 1 ,MPI_INT, i, 2, MPI_COMM_WORLD);
        MPI_Send(&vertex[begin], end-begin, MPI_INT, i, 3, MPI_COMM_WORLD);
    }
    int begin=(activeProc-1)*batchSize;
    int end=vertex.size();
    MPI_Send(&begin, 1 , MPI_INT, activeProc, 1,MPI_COMM_WORLD);
    MPI_Send(&end, 1 , MPI_INT, activeProc, 2, MPI_COMM_WORLD);
    MPI_Send(&vertex[begin], end-begin, MPI_INT, activeProc, 3, MPI_COMM_WORLD);
}


void reciveVertexNumberFromMaster(vector<int>& vertex)
{
    MPI_Status Status;
    int begin, end;
    MPI_Recv(&begin, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &Status);
    MPI_Recv(&end, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &Status);
    vertex.resize(end-begin);
    MPI_Recv(&vertex[0], end-begin, MPI_INT, 0, 3, MPI_COMM_WORLD, &Status);
}

void parallelJonesPlassmansColoring(Graph &graph, int seed, int ProcRank, int ProcNum)
{
    MPI_Status Status;
    vector<int> randomNumber;
    srand(seed);
    for(int i=0;i<graph.vertexCount;i++)
    {
        randomNumber.push_back(rand());
    }

    if(ProcRank==0)
    {
        int activeProcessCount;
        sendVertexNumberToProcess(graph.vertexCount, ProcNum, activeProcessCount);
        vector<int> activeProcess;
        for(int i=1;i<=activeProcessCount;i++)
        {
            activeProcess.push_back(i);
        }

        int remainVertexCount=graph.vertexCount;
        while(remainVertexCount!=0)
        {
            vector<int> newActiveProcess;

            for(unsigned int i=0;i<activeProcess.size();i++)
            {
                int changedVertexCount;
                MPI_Recv(&changedVertexCount, 1, MPI_INT, activeProcess[i], 4, MPI_COMM_WORLD, &Status);

                if(changedVertexCount!=-1)
                {
                    if(changedVertexCount>0)
                    {
                        vector<int> changedVertex;
                        changedVertex.resize(changedVertexCount);
                        MPI_Recv(&changedVertex[0], changedVertexCount, MPI_INT, activeProcess[i], 5, MPI_COMM_WORLD, &Status);
                        vector<int> colors;
                        colors.resize(graph.vertexCount);
                        MPI_Recv(&colors[0], graph.vertexCount, MPI_INT, activeProcess[i], 6, MPI_COMM_WORLD, &Status);
                        for(unsigned int j=0;j<changedVertex.size();j++)
                        {
                            graph.colors[changedVertex[j]]=colors[changedVertex[j]];
                        }
                    }
                    MPI_Send(&(graph.colors[0]), graph.vertexCount, MPI_INT, activeProcess[i],10, MPI_COMM_WORLD);
                    remainVertexCount-=changedVertexCount;
                    newActiveProcess.push_back(activeProcess[i]);
                }
            }

            activeProcess=newActiveProcess;
        }
    }
    else
    {
        vector<int> remainVertex;
        reciveVertexNumberFromMaster(remainVertex);

        vector<int> changedVertex;
        while(!remainVertex.empty())
        {
            changedVertex.clear();
            vector<int> newRemainVertex;
            for(unsigned int i=0; i<remainVertex.size();i++)
            {
                vector<int> neighbors;

                graph.findNeighbors(remainVertex[i], neighbors);
                vector<int> unColoredNeighborsNumber;
                for(unsigned int j=0;j<neighbors.size();j++)
                {
                    if(graph.colors[neighbors[j]]==-1)
                    {
                        unColoredNeighborsNumber.push_back(randomNumber[neighbors[j]]);
                    }
                }

                if(unColoredNeighborsNumber.empty())
                {
                    graph.colors[remainVertex[i]]=graph.smallestNeighborsColor(remainVertex[i]);
                    changedVertex.push_back(remainVertex[i]);
                }
                else
                {
                    int maxNeighborNumber=*std::max_element(unColoredNeighborsNumber.begin(), unColoredNeighborsNumber.end());


                    int indexMaxNeighbor=0;
                    for(unsigned int j=0;j<neighbors.size();j++)
                    {
                        if(randomNumber[neighbors[j]]==maxNeighborNumber && graph.colors[neighbors[j]]!=-1)
                        {
                            indexMaxNeighbor=j;
                        }
                    }
                    if(maxNeighborNumber<randomNumber[remainVertex[i]] || (maxNeighborNumber==randomNumber[remainVertex[i]] && indexMaxNeighbor<remainVertex[i]))
                    {
                        graph.colors[remainVertex[i]]=graph.smallestNeighborsColor(remainVertex[i]);
                        changedVertex.push_back(remainVertex[i]);
                    }
                    else
                    {
                        newRemainVertex.push_back(remainVertex[i]);
                    }
                }
            }
            remainVertex=newRemainVertex;
            int changedVertexCount=changedVertex.size();
            MPI_Send(&changedVertexCount, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
            if(changedVertexCount>0)
            {
                MPI_Send(&changedVertex[0], changedVertexCount, MPI_INT, 0, 5, MPI_COMM_WORLD);
                MPI_Send(&(graph.colors[0]), graph.vertexCount, MPI_INT, 0, 6, MPI_COMM_WORLD);
            }

            MPI_Recv(&(graph.colors[0]), graph.vertexCount, MPI_INT, 0,10, MPI_COMM_WORLD,&Status);
        }
        int flagEnd=-1;
        MPI_Send(&flagEnd, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
    }
}

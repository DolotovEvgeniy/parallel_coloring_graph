#include  <mpi.h>
#include <stdlib.h>
#include <assert.h>
#include "graph.h"
#include <iostream>
#include <vector>
#include "coloring_algorithm.h"
using namespace std;

#define PRINT_VERTEX_COUNT 10

double serialStart;
double serialTotalTime;

double parallelStart;
double parallelTotalTime;

double greedyColoringStart;
double greedyColoringTotalTime;

int main(int argc, char *argv[])
{
    int ProcRank, ProcNum;
    MPI_Status Status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD,&ProcRank);


    int vertexCount=atoi(argv[1]);
    assert(vertexCount>0);

    int seed=atoi(argv[2]);

    Graph graph(vertexCount, seed);



    if(ProcRank==0)
    {
        if(graph.vertexCount<PRINT_VERTEX_COUNT)
        {
            graph.printGraph();
        }
    }

    if(ProcRank==0)
    {

        cout<<"------GREEDY ALGORITHM------"<<endl;
        greedyColoringStart=MPI_Wtime();
        greedyColoring(graph);
        greedyColoringTotalTime=MPI_Wtime()-greedyColoringStart;

        if(graph.vertexCount<PRINT_VERTEX_COUNT)
        {
            graph.printColors();
        }

        graph.checkColors();

        cout<<"Colors count: "<<graph.colorsCount()<<endl;
        cout<<"Time: "<<greedyColoringTotalTime<<endl;
        cout<<"----------------------------"<<endl;
    }

    if(ProcRank==0)
    {

        cout<<"------JONES-PLASSMANS SERIAL ALGORITHM------"<<endl;
        graph.clearColors();
        serialStart=MPI_Wtime();
        jonesPlassmansColoring(graph, seed);
        serialTotalTime=MPI_Wtime()-serialStart;

        if(graph.vertexCount<PRINT_VERTEX_COUNT)
        {
            graph.printColors();
        }

        graph.checkColors();

        cout<<"Colors count: "<<graph.colorsCount()<<endl;
        cout<<"Time: "<<serialTotalTime<<endl;
        cout<<"----------------------------"<<endl;
    }
    vector<int> randomNumber;

    if(ProcRank==0)
    {
        cout<<"------JONES-PLASSMANS PARALLEL ALGORITHM------"<<endl;
        graph.clearColors();
        parallelStart=MPI_Wtime();
        parallelJonesPlassmansColoring(graph, seed, ProcRank, ProcNum);
        parallelTotalTime=MPI_Wtime()-parallelStart;

        if(graph.vertexCount<PRINT_VERTEX_COUNT)
        {
            graph.printColors();
        }

        graph.checkColors();

        cout<<"Colors count: "<<graph.colorsCount()<<endl;
        cout<<"Time: "<<parallelTotalTime<<endl;
        cout<<"----------------------------"<<endl;
    }
    else
    {
        parallelJonesPlassmansColoring(graph, seed, ProcRank, ProcNum);
    }

    if(ProcRank==0)
    {
        cout<<"---------STATISTIC---------"<<endl;
        cout<<"Parallel speed up:"<< serialTotalTime/parallelTotalTime<<endl;
        cout<<"---------------------------"<<endl;
    }

    MPI_Finalize();
    return 0;
}

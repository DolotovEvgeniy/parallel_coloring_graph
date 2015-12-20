#ifndef COLORING_ALGORITHM_H
#define COLORING_ALGORITHM_H
#include "graph.h"

void greedyColoring(Graph& graph);

void jonesPlassmansColoring(Graph& graph, int seed);

void parallelJonesPlassmansColoring(Graph& graph, int seed, int ProcRank, int ProcNum);

#endif // COLORING_ALGORITHM_H

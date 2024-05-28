#pragma once
#include <vector>
#include <limits>
#include <mpi.h>

#define INF std::numeric_limits<int>::max()

namespace mpi {
	std::vector<int> GFG(const int* graph, int node_count, int connection_count, int start, int node_begin, int node_per_process);
}
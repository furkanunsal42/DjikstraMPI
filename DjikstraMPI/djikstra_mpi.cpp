#include "djikstra_mpi.h"

#include <iostream>
#include <vector>

std::vector<int> mpi::GFG(const int* graph, int node_count, int connection_count, int start, int node_begin, int node_per_process) {
    
    int num_nodes = node_count;
    std::vector<int> dist(num_nodes, INF);
    std::vector<bool> visited(num_nodes, false);
    dist[start] = 0;
    
    for (int i = node_begin; i < node_begin + node_per_process; ++i) {
        int u = -1, min_dist = INF;
        // Find the node with the shortest distance
        for (int j = 0; j < connection_count; ++j) {
            if (!visited[j] && dist[j] < min_dist) {
                u = j;
                min_dist = dist[j];
            }
        }
        if (u == -1) break;
        
        // Relax adjacent nodes
        for (int v = 0; v < num_nodes; ++v) {
            if (!visited[v] && graph[u + connection_count * v] && dist[u] + graph[u + connection_count * v] < dist[v]) {
                dist[v] = dist[u] + graph[u + connection_count * v];
            }
        }
        visited[u] = true;
    }

    // Print the shortest distances
    std::cout << "Vertex \t Distance from Source\n";
    for (int i = node_begin; i < node_begin + node_per_process; ++i) {
        std::cout << i << "\t\t" << dist[i] << "\n";
    }

    return dist;
}

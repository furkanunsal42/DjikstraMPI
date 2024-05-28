#include "djikstra_omp.h"

#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>

void omp::GFG(const std::vector<std::vector<int>>& graph, int start) {
    int num_nodes = graph.size();
    std::vector<int> dist(num_nodes, INF);
    std::vector<bool> visited(num_nodes, false);
    dist[start] = 0;
    #pragma omp parallel for
    for (int i = 0; i < num_nodes; ++i) {
        int u = -1, min_dist = INF;
        // Find the node with the shortest distance
        for (int j = 0; j < num_nodes; ++j) {
            if (!visited[j] && dist[j] < min_dist) {
                u = j;
                min_dist = dist[j];
            }
        }
        if (u == -1) break;
        // Relax adjacent nodes
        for (int v = 0; v < num_nodes; ++v) {
            if (!visited[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
        visited[u] = true;
    }
    // Print the shortest distances
    std::cout << "Vertex \t Distance from Source\n";
    for (int i = 0; i < num_nodes; ++i) {
        std::cout << i << "\t\t" << dist[i] << "\n";
    }
}

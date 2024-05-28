#include "djikstra_mpi.h"
#include "djikstra_omp.h"

#include <iostream>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* my_graph = new int[9 * 9];
    int node_begin;
    int node_per_process;

    int* cluster = new int[9] {0, 0, 0, 0, 0, 0, 0, 0};

    int cluster_vertex_amount = 0;
    
    if (rank == 0) {
        int* graph = new int[9 * 9] {
            0, 4, 0, 0, 0, 0, 0, 8, 0,
            4, 0, 8, 0, 0, 0, 0, 11, 0,
            0, 8, 0, 7, 0, 4, 0, 0, 2,
            0, 0, 7, 0, 9, 14, 0, 0, 0,
            0, 0, 0, 9, 0, 10, 0, 0, 0,
            0, 0, 4, 14, 10, 0, 2, 0, 0,
            0, 0, 0, 0, 0, 2, 0, 1, 6,
            8, 11, 0, 0, 0, 0, 1, 0, 7,
            0, 0, 2, 0, 0, 0, 6, 7, 0
        };
        my_graph = graph;

        node_begin = 0;
        node_per_process = 3;

        for (int i = 1; i < 3; i++) {
            MPI_Send(graph, 9 * 9, MPI_INT, i, 0, MPI_COMM_WORLD);

            int worker_node_begin = i * node_per_process;

            MPI_Send(&worker_node_begin, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&node_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        }
    }
    else {
        MPI_Status status;
        MPI_Recv(my_graph, 9 * 9, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&node_begin, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&node_per_process, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int start = 0;
    int num_nodes = 9;
    int connection_count = 9;

    std::vector<int> dist(num_nodes, INF);
    std::vector<bool> visited(num_nodes, false);
    dist[start] = 0;

    for (int i = 0; i < 9; i++) {
        int closest_vertex;
        int closest_distance;
        for (int i = node_begin; i < node_begin + node_per_process; ++i) {
            closest_vertex = -1, closest_distance = INF;
            // Find the node with the shortest distance
            for (int j = 0; j < connection_count; ++j) {
                if (!visited[j] && dist[j] < closest_distance) {
                    closest_vertex = j;
                    closest_distance = dist[j];
                }
            }
        }

        if (rank == 0) {
        
            for (int i = 1; i < 3; i++) {
                int locally_closest_distance = -1;
                int locally_closest_vertex = -1;
                MPI_Status status;
                MPI_Recv(&closest_distance, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&locally_closest_vertex, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                if (locally_closest_vertex < 0) continue;
                if (locally_closest_distance <= 0) continue;
                if (locally_closest_distance < closest_distance) {
                    closest_distance = locally_closest_distance;
                    closest_vertex = locally_closest_vertex;
                }
            }
        }
        else {
            MPI_Send(&closest_distance, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&closest_vertex, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        for (int i = node_begin; i < node_begin + node_per_process; ++i)
        {
            if (closest_vertex == -1) break;

            // Relax adjacent nodes
            for (int v = 0; v < num_nodes; ++v) {
                if (!visited[v] && my_graph[closest_vertex + connection_count * v] && dist[closest_vertex] + my_graph[closest_vertex + connection_count * v] < dist[v]) {
                    dist[v] = dist[closest_vertex] + my_graph[closest_vertex + connection_count * v];
                }
            }
            visited[closest_vertex] = true;
        }
    }
    

    // Print the shortest distances
    std::cout << "Vertex \t Distance from Source\n";
    for (int i = node_begin; i < node_begin + node_per_process; ++i) {
        std::cout << i << "\t\t" << dist[i] << "\n";
    }

    MPI_Finalize();

    return 0;
}
#include "djikstra_mpi.h"
#include "djikstra_omp.h"

#include <iostream>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* my_graph = new int[9*9];
    int node_begin;
    int node_per_process;

    if (rank == 0) {
        int* graph = new int[9*9] {
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

    mpi::GFG(my_graph, 9, 9, 0, node_begin, node_per_process);

    MPI_Finalize();

    return 0;
}
/*

*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

/*  ---------------------------------- STRUCTS -----------------------------  */

typedef struct data {
    unsigned int start_line, chunk, exced;
} Data;

/*  ---------------------------------- DEFINES -----------------------------  */

#define is_root(m)  ((0 == m) ? 1 : 0)

#define start(d)    (d.start_line)
#define chunk(d)    (d.chunk)
#define exced(d)    (d.exced)

/*  ----------------------------------- MAIN -------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    int matrix[2][3] = {{1, 1, 2}, {2, 2, 4}}, matrix_size = 10, tag = 0;
    Data data;
    /*  Variáveis OMP.  */
    // int n_threads = 0, this_thread = 0;
    /*  Variavéis MPI.  */
    int world_size = 0, world_rank = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    if (is_root(world_rank)) {
        /*  Caso a matrix não consiga ser dividida igualmente entre os processos, o último arcará com o excesso */
        exced(data) = matrix_size%world_size;
        chunk(data) = matrix_size/world_size;
        start(data) = 0;
    } else {
        MPI_Recv(&data, 1, MPI_BYTE, world_rank-1, tag, MPI_COMM_WORLD, status);
        start(data) = start(data) + chunk(data);        
    }
    
    MPI_Send(&data, 1, MPI_BYTE, (world_rank+1)%world_size, tag, MPI_COMM_WORLD);
    printf("[Process %d]\nstart: %d\nchunck: %d\nexced: %d\n\n", world_rank, start(data), chunk(data), exced(data));

    /*
    #pragma omp parallel default(shared) private(n_threads, this_thread)
    {
        n_threads = omp_get_num_threads();
        this_thread = omp_get_thread_num();

        printf("[Process %d] Hello from thread #%d of %d\n", world_rank, this_thread, n_threads);
    }
    */

    if (is_root(world_rank)) {
    }

    MPI_Finalize();
    
    return 0;
}

/*
    Cuidado ao verificar as questões de divizões por quando o pivot for igual a zero.
*/
#include "matrix.h"
#include "GJ.h"

/*  -------------------------------------------------- MAIN --------------------------------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    Data *data = NULL;
    /*  Variáveis MPI.  */
    int world_size = 0, world_rank = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    data = matrix_read("matriz.txt", "vetor.txt");
    
    if (is_root(world_rank)) {
        printf("\nORIGINAL MATRIX:\n");
        print_matrix(data);
    }

    pivoting(world_rank, world_size, data);
    printf("#%d PROCESS\n", world_rank);
    print_matrix(data);
    // merge_matrix(world_rank, world_size, data);

    if (is_tail(world_rank, world_size)) {
        // printf("\nBEFORE CLEANING COLLUMNS:\n");
        // print_matrix(data);
        // clear_columns(data);
        // printf("\nFINAL MATRIX:\n");
        // print_matrix(data);
    }

    MPI_Finalize();
    
    free_matrix(&data);

    return 0;
}

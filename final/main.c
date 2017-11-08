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

    data = matrix_read("matriz.txt", "resultado.txt");
    
    if (is_root(world_rank)) {
        printf("\nORIGINAL MATRIX:\n");
        // print_matrix(data);
    }

    //pivoting(world_rank, world_size, matrix, matrix_size);
    //merge_matrix(world_rank, world_size, matrix, matrix_size);

    if (is_tail(world_rank, world_size)) {
        printf("\nBEFORE CLEANING COLLUMNS:\n");
        // print_matrix(data);
        //clear_columns(matrix, matrix_size);
        printf("\nFINAL MATRIX:\n");
        //print_matrix(matrix, matrix_size);
    }

    MPI_Finalize();
    
    //free_matrix(&matrix, matrix_size);

    return 0;
}

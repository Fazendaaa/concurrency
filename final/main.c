/*
    Cuidado ao verificar as questões de divizões por quando o pivot for igual a zero.
*/
#include "GJ.h"
#include "utils.h"
#include "matrix.h"

/*  -------------------------------------------------- MAIN --------------------------------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    FILE *src = NULL;
    size_t matrix_size = atoi(argv[1]);
    float **matrix = createMatrix(matrix_size);
    /*  Variáveis MPI.  */
    int world_size = 0, world_rank = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    readMatrix(&matrix, matrix_size);
    
    if (is_root(world_rank)) {
        printf("\nORIGINAL MATRIX:\n");
        printMatrix(matrix, matrix_size);
    }

    // pivoting(world_rank, world_size, matrix, matrix_size);
    // merge_matrix(world_rank, world_size, matrix, matrix_size);

    if (is_tail(world_rank, world_size)) {
        printf("\nBEFORE CLEANING COLLUMNS:\n");
        printMatrix(matrix, matrix_size);
        // clear_columns(matrix, matrix_size);
        printf("\nFINAL MATRIX:\n");
        printMatrix(matrix, matrix_size);
    }

    MPI_Finalize();
    
    freeMatrix(&matrix, matrix_size );

    return 0;
}

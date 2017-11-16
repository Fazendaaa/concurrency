/*
    Cuidado ao verificar as questões de divizões por quando o pivot for igual a zero.
*/
#include "GJ.h"

/*  ------------------------------------------------- HEADERS ------------------------------------------------------  */

static int ** matrix_init (const size_t);
static void print_matrix (int **, const size_t);
static void free_matrix (int ***, const size_t );

/*  -------------------------------------------------- MAIN --------------------------------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    size_t matrix_size = 6;
    int **matrix = NULL;
    /*  Variáveis MPI.  */
    int world_size = 0, world_rank = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    matrix = matrix_init(matrix_size);

    if (is_root(world_rank)) {
        printf("\nORIGINAL MATRIX:\n");
        print_matrix(matrix, matrix_size);
    }

    swapping(world_rank, world_size, matrix, matrix size);

    pivoting(world_rank, world_size, matrix, matrix_size);
    merge_matrix(world_rank, world_size, matrix, matrix_size);

    if (is_tail(world_rank, world_size)) {
        printf("\nBEFORE CLEANING COLLUMNS:\n");
        print_matrix(matrix, matrix_size);
        clear_columns(matrix, matrix_size);
        printf("\nFINAL MATRIX:\n");
        print_matrix(matrix, matrix_size);
    }

    MPI_Finalize();

    free_matrix(&matrix, matrix_size);

    return 0;
}

/*  --------------------------------------------- IMPLEMENTATIONS --------------------------------------------------  */

/*
*   Apenas inicializa uma matriz com dummy data.
*/
static int ** matrix_init (const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0;
    int **matrix = (int**) malloc(sizeof(int*) * matrix_line);
    int val = 0;

    if (NULL != matrix) {
        for (; i < matrix_line; i++) {
            matrix[i] = (int*) malloc(sizeof(int) * matrix_col);
            val = i;

            for (j = 0; j < matrix_col; j++) {
                matrix[i][j] = val++;
            }
        }
    }

    return matrix;
}

/*
*   Imprime uma matrix do tipo NxN+1.
*/
static void print_matrix (int ** matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0;

    if (NULL != matrix) {
        for (; i < matrix_line; i++) {
            for (j = 0; j < matrix_col; j++) {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

/*
*   Libera o espaço alocado da matrix da mememória.
*/
static void free_matrix (int ***matrix, const size_t size) {
    size_t i = 0;

    if (NULL != matrix && NULL != *matrix) {
        for (; i < size; i++) {
            free((*matrix)[i]);
        }
        free (*matrix);
        *matrix = NULL;
    }
}

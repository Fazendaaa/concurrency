/*

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

/*  ------------------------------- DEFINES --------------------------------  */

#define is_root(m)      ((0 == m) ? 1 : 0)
#define is_tail(p, w)   ((p == w-1) ? 1 : 0)

/*  -------------------------------- HEADERS -------------------------------  */

static int ** matrix_init (const size_t);
static void print_matrix (int **, const size_t);
static void free_matrix (int ***, const size_t );

static void pivoting (const int, const int, int**, const size_t);
static void merge_matrix (const int, const int, int**, const size_t);

/*  --------------------------------- MAIN ---------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    size_t matrix_size = 4;
    int **matrix = NULL;
    /*  Variáveis MPI.  */
    int world_size = 0, world_rank = 0;
    /*  Variáveis OMP.  */
    //int this_thread = 0, n_threads = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    matrix = matrix_init(matrix_size);
    
    if (is_root(world_rank)) {
        printf("\nORIGINAL MATRIX:\n");
        print_matrix(matrix, matrix_size);
    }

    pivoting(world_rank, world_size, matrix, matrix_size);

    /*  Aqui  deve ir o código que junta todas as linhas pivotadas em uma matriz
        só para o processo tail poder zerar as colunas.
    */
    merge_matrix(world_rank, world_size, matrix, matrix_size);

    if (is_tail(world_rank)) {
        printf("\nFINAL MATRIX:\n");
        print_matrix(matrix, matrix_size);
        /*  Zerar as colunas aqui.  */
    }

    MPI_Finalize();
    
    free_matrix(&matrix, matrix_size);

    return 0;
}

/*  ---------------------------- IMPLEMENTATIONS ---------------------------  */

static int ** matrix_init (const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0;
    int **matrix = (int**) malloc(sizeof(int*) * matrix_line);
    int val = 0;

    if (NULL != matrix) {
        for (; i < matrix_line; i++) {
            matrix[i] = (int*) malloc(sizeof(int) * matrix_col);
            val = i+2;

            for (j = 0; j < matrix_col; j++) {
                matrix[i][j] = val;
            }
        }
    }

    return matrix;
}

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

static void pivoting (const int world_rank, const int world_size,
                      int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, i = 0, j = 0;
    size_t chunk = matrix_size/world_size, limit = 0, pivot = 0;

    if (NULL != matrix) {
        /*  Calcula  até  qual  linha o processo designador será responsável por
            pivotá-la.  */
        limit = (world_rank+1)*chunk;
        
        /*  Cada  processo  fica reponsável pela sua quantidade de linhas apenas
            para pivotamento.   */
        for (i = world_rank*chunk; i < limit; i++) {
            pivot = matrix[i][i];

            /*  Como  não  há  interdependência  dos  dados  nesse  for, se pode
                paralelizar  a  tarefa de dividir a linha pelo pivot sem maiores
                preocupações com dependência dos valores.
            */
            #pragma omp parallel for
            for (j = 0; j < matrix_col; j++) {
                matrix[i][j] /= pivot;
            }
        }

        printf("\n#%d PROCESS:\n", world_rank);
        print_matrix(matrix, matrix_size);
    }
}

static void matrix_to_vector (const int **matrix, int *vector,
                             const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0,
           k = 0;

    if (NULL != matrix && NULL != vector) {
        for (; i < matrix_line; i++) {
            for (j = 0; j < matrix_col; j++) {
                vector[k++] = matrix[i][j];
            }
        }
    }
}

static void vector_to_matrix (const int *vector, int **matrix, 
                             const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0,
           k = 0;

    if (NULL != matrix && NULL != vector) {
        for (; i < matrix_line; i++) {
            for (j = 0; j < matrix_col; j++) {
                matrix[i][j] = vector[k++];
            }
        }
    }
}

static void merge_pivoting (const int world_rank, const int world_size,
                            int **matrix, int *vecotr,
                            const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size;
    
}

static void merge_matrix (const int world_rank, const int world_size,
                          int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size;
    size_t chunk = matrix_size/world_size;
    int *vector = (int *) malloc(sizeof(int) * (matrix_line*matrix_col));
    
    /*  Uma  estrutura de anel para passar as linhas do processo anterior que já
        foram  pivotadas  com  a  do  processo  atual  e  passar  para o próximo
        processo.   */
    if (is_root(world_rank)) {
        matrix_to_vector(matrix, vector, matrix_size);
        MPI_Send(vector, matrix_line*matrix_col, MPI_INT,
                (world_rank+1)%world_size, 0, MPI_STATUS_IGNORE);        
    } else if (!is_tail(world_rank, world_size)){
        MPI_Recv(vector, matrix_line*matrix_col, MPI_INT, world_size-1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /*  Juntar  a  matrix  com  as linhas pivotadas até este processo com as
            pivotadas por este processo.    */
        merge_pivoting(world_rank, world_size, matrix, vector, matrix_size);
        matrix_to_vector(matrix, vector, matrix_size);
        MPI_Send(vector, matrix_line*matrix_col, MPI_INT,
                (world_rank+1)%world_size, 0, MPI_STATUS_IGNORE);
    } else {
        /*  Quando  o  processo for o tail, ele apenas juntará toda a informação
            em  uma matriz final que será utilizada posteriormente para zerar as
            colunas.    */
        MPI_Recv(vector, matrix_line*matrix_col, MPI_INT, world_size-1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        merge_pivoting(world_rank, world_size, matrix, vector, matrix_size);
        matrix_to_vector(matrix, vector, matrix_size);
        vector_to_matrix(vector, matrix);
    }

    free(vector);
}

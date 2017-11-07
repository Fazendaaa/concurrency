/*

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

/*  ------------------------------- DEFINES --------------------------------  */

#define is_root(p)      ((0 == p) ? 1 : 0)
#define is_tail(p, w)   ((w-1 == p) ? 1 : 0)

/*  -------------------------------- HEADERS -------------------------------  */

static int ** matrix_init (const size_t);
static void print_matrix (int **, const size_t);
static void free_matrix (int ***, const size_t );

static void pivoting (const int, const int, int**, const size_t);
static void merge_matrix (const int, const int, int**, const size_t);
static void clear_columns (int**, const size_t);

/*  --------------------------------- MAIN ---------------------------------  */

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

/*  ---------------------------- IMPLEMENTATIONS ---------------------------  */

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

            /*  Caso  o  pivot  seja zero, o sistema no final poderá ser do tipo
                possível, todavia, indeterminado.   */
            if (0 != pivot) {
                /*  Como  há  interdependência  dos  dados  nesse  loop, se pode
                    paralelizar  a  tarefa  de  dividir  a  linha pelo pivot sem
                    maiores preocupações com dependência dos valores.   */
                #pragma omp parallel for
                for (j = 0; j < matrix_col; j++) {
                    matrix[i][j] /= pivot;
                }
            }
        }

        printf("\n#%d PROCESS:\n", world_rank);
        print_matrix(matrix, matrix_size);
    }
}

static void matrix_to_vector (int **matrix, int *vector,
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

static void vector_to_matrix (int *vector, int **matrix, 
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
                            int ** matrix, int *vector,
                            const size_t matrix_size) {
    size_t chunk = matrix_size/world_size, limit = 0;
    size_t matrix_col = matrix_size+1, i = 0, j = 0, k = 0;

    if (NULL != matrix && NULL != vector) {
        limit = (world_rank+1)*chunk;
        k = (world_rank*chunk)*matrix_col;

        for (i = world_rank*chunk; i < limit; i++) {
            for (j = 0; j < matrix_col; j++) {
                vector[k++] = matrix[i][j];
            }
        }
    }
}

static void merge_matrix (const int world_rank, const int world_size,
                          int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size;
    int *vector = (int *) malloc(sizeof(int) * (matrix_line*matrix_col));
    
    /*  Uma  estrutura de anel para passar as linhas do processo anterior que já
        foram  pivotadas  com  a  do  processo  atual  e  passar  para o próximo
        processo.   */
    if (is_root(world_rank)) {
        matrix_to_vector(matrix, vector, matrix_size);
        MPI_Send(vector, matrix_line*matrix_col, MPI_INT,
                (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
    } else if (!is_tail(world_rank, world_size)) {
        MPI_Recv(vector, matrix_line*matrix_col, MPI_INT, world_rank-1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /*  Juntar  a  matrix  com  as linhas pivotadas até este processo com as
            pivotadas por este processo.    */
        merge_pivoting(world_rank, world_size, matrix, vector, matrix_size);
        MPI_Send(vector, matrix_line*matrix_col, MPI_INT,
                (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
    } else {
        /*  Quando  o  processo for o tail, ele apenas juntará toda a informação
            em  uma matriz final que será utilizada posteriormente para zerar as
            colunas.    */
        MPI_Recv(vector, matrix_line*matrix_col, MPI_INT, world_rank-1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        merge_pivoting(world_rank, world_size, matrix, vector, matrix_size);
        vector_to_matrix(vector, matrix, matrix_size);
    }

    free(vector);
}

static void clear_columns (int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0,
           k = 0, pivot = 0;
    float factor = 0;
    
    if (NULL != matrix) {
        /*  Uma linha de cada vez da matrix será selecionada para zerar a coluna
            do seu pivot nas outras linhas. */
        for (; i < matrix_line; i++) {
            pivot = matrix[i][i];

            /*  O  pivot  será  zero  quando  alguma chamada anterior acabou por
                zerar a sua posição.    */
            if (0 != pivot) {
                for (j = 0; j < matrix_line; j++) {
                    /*  Não  faz  sentido  procurar  zerar  a coluna na linha do
                        pivot.*/
                    if (i != j) {
                        factor = matrix[j][i]/pivot;
    
                        /*  Na  linha  que  se  busca zerar a coluna, subtrair a
                            linha do pivot. */
                        #pragma omp parallel for
                        for (k = 0; k < matrix_col; k++) {
                            matrix[j][k] -= factor*matrix[i][k];
                        }
                    }
                }
            }
        }
    }
}

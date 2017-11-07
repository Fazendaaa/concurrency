#include "GJ.h"

/*  --------------------------------------------- IMPLEMENTATIONS --------------------------------------------------  */

/*
*   Dada  uma matrix e o id do processo, essa função irá dividir as linhas responsáveis pelo processo pelo valor de seus
*   respectivos pivots, o que irá que sua diagonal seja igual a um.
*/
void pivoting (const int world_rank, const int world_size, int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, i = 0, j = 0;
    size_t chunk = matrix_size/world_size, limit = 0, pivot = 0;

    if (NULL != matrix) {
        /*  Calcula até qual linha o processo designado será responsável por pivotá-la. */
        limit = (world_rank+1)*chunk;
        
        /*  Cada processo fica reponsável pela sua quantidade de linhas apenas para pivotamento.    */
        #pragma omp parallel for
        for (i = world_rank*chunk; i < limit; i++) {
            pivot = matrix[i][i];

            /*  Caso o pivot seja zero, o sistema no final poderá ser do tipo possível, todavia, indeterminado. */
            if (0 != pivot) {
                /*  Como  há interdependência dos dados nesse loop, se pode paralelizar a tarefa de dividir a linha pelo
                    pivot sem maiores preocupações com dependência dos valores.   */
                #pragma omp parallel for
                for (j = 0; j < matrix_col; j++) {
                    matrix[i][j] /= pivot;
                }
            }
        }
    }
}

/*
*   Transforma um array 2d em um array 1d.
*/
static void matrix_to_vector (int **matrix, int *vector, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0, k = 0;

    if (NULL != matrix && NULL != vector) {
        for (; i < matrix_line; i++) {
            for (j = 0; j < matrix_col; j++) {
                vector[k++] = matrix[i][j];
            }
        }
    }
}

/*
*   Transforma um array 1d em um array 2d.
*/
static void vector_to_matrix (int *vector, int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0, k = 0;

    if (NULL != matrix && NULL != vector) {
        for (; i < matrix_line; i++) {
            for (j = 0; j < matrix_col; j++) {
                matrix[i][j] = vector[k++];
            }
        }
    }
}

/*
*   Junta a matrix que possui os pivotamentos anteriormente realizados com o atual.
*/
static void merge_pivoting (const int world_rank, const int world_size, int ** matrix, int *vector,
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

/*
*   Junta  todos os pivotamentos realizados om o da matrix que o processo responsável pivotou, em uma estrutura de anel.
*   Cada processo fica responsável por pivotar um número de linhas de maneira crescente ao seu ID.
*/
void merge_matrix (const int world_rank, const int world_size, int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size;
    int *vector = (int *) malloc(sizeof(int) * (matrix_line*matrix_col));
    
    /*  Uma estrutura de anel para passar as linhas do processo anterior que já foram pivotadas com a do processo atual
        e passar para o próximo processo.   */
    if (is_root(world_rank)) {
        matrix_to_vector(matrix, vector, matrix_size);
        MPI_Send(vector, matrix_line*matrix_col, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
    } else if (!is_tail(world_rank, world_size)) {
        MPI_Recv(vector, matrix_line*matrix_col, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        /*  Juntar a matrix com as linhas pivotadas até este processo com as pivotadas por este processo.    */
        merge_pivoting(world_rank, world_size, matrix, vector, matrix_size);
        MPI_Send(vector, matrix_line*matrix_col, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
    } else {
        /*  Quando  o  processo  for o tail, ele apenas juntará toda a informação em uma matriz final que será utilizada
            posteriormente para zerar as colunas.   */
        MPI_Recv(vector, matrix_line*matrix_col, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        merge_pivoting(world_rank, world_size, matrix, vector, matrix_size);
        vector_to_matrix(vector, matrix, matrix_size);
    }

    free(vector);
}

/*
*   Dada a matriz já pivotada, se zera as colunas desses pivots.
*/
void clear_columns (int **matrix, const size_t matrix_size) {
    size_t matrix_col = matrix_size+1, matrix_line = matrix_size, i = 0, j = 0, k = 0, pivot = 0;
    float factor = 0;
    
    if (NULL != matrix) {
        /*  Uma linha de cada vez da matrix será selecionada para zerar a coluna do seu pivot nas outras linhas.    */
        for (; i < matrix_line; i++) {
            pivot = matrix[i][i];

            /*  O pivot será zero quando alguma chamada anterior acabou por zerar a sua posição.    */
            if (0 != pivot) {
                for (j = 0; j < matrix_line; j++) {
                    /*  Não faz sentido procurar zerar a coluna na linha do prórprio pivot. */
                    if (i != j) {
                        factor = matrix[j][i]/pivot;
    
                        /*  Na linha que se busca zerar a coluna, subtrair a linha do pivot.    */
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

#include "GJ.h"

/*  --------------------------------------------- IMPLEMENTATIONS --------------------------------------------------  */

/*
*   Dada  uma matrix e o id do processo, essa função irá dividir as linhas responsáveis pelo processo pelo valor de seus
*   respectivos pivots, o que irá que sua diagonal seja igual a um.
*/
void pivoting (const int world_rank, const int world_size, Data *data) {
    size_t chunk = 0, limit = 0, i = 0, j = 0;
    float pivot = 0;

    if (NULL != data) {
        chunk = line(data)/world_size;
        /*  Calcula até qual linha o processo designado será responsável por pivotá-la. */
        limit = (world_rank+1)*chunk;
        
        /*  Cada processo fica reponsável pela sua quantidade de linhas apenas para pivotamento.    */
        #pragma omp parallel for
        for (i = world_rank*chunk; i < limit; i++) {
            pivot = matrix(data)[i][i];

            /*  Caso o pivot seja zero, o sistema no final poderá ser do tipo possível, todavia, indeterminado. */
            if (0 != pivot) {
                /*  Como  há interdependência dos dados nesse loop, se pode paralelizar a tarefa de dividir a linha pelo
                    pivot sem maiores preocupações com dependência dos valores.   */
                #pragma omp parallel for
                for (j = 0; j < col(data); j++) {
                    matrix(data)[i][j] /= pivot;
                }
            }
        }
    }
}

/*
*   Transforma um array 2d em um array 1d.
*/
static void matrix_to_vector (Data *data, float *vector) {
    size_t i = 0, j = 0, k = 0;

    if (NULL != data && NULL != vector) {
        for (; i < line(data); i++) {
            for (j = 0; j < col(data); j++) {
                vector[k++] = matrix(data)[i][j];
            }
        }
    }
}

/*
*   Transforma um array 1d em um array 2d.
*/
static void vector_to_matrix (Data *data, float *vector) {
    size_t i = 0, j = 0, k = 0;

    if (NULL != data && NULL != vector) {
        for (; i < line(data); i++) {
            for (j = 0; j < col(data); j++) {
                matrix(data)[i][j] = vector[k++];
            }
        }
    }
}

/*
*   Junta a matrix que possui os pivotamentos anteriormente realizados com o atual.
*/
static void merge_pivoting (const int world_rank, const int world_size, Data *data, float *vector) {
    size_t chunk = 0, limit = 0, i = 0, j = 0, k = 0;

    if (NULL != data && NULL != vector) {
        chunk = line(data)/world_size;
        limit = (world_rank+1)*chunk;
        k = (world_rank*chunk)*col(data);

        for (i = world_rank*chunk; i < limit; i++) {
            for (j = 0; j < col(data); j++) {
                vector[k++] = matrix(data)[i][j];
            }
        }
    }
}

/*
*   Junta  todos os pivotamentos realizados om o da matrix que o processo responsável pivotou, em uma estrutura de anel.
*   Cada processo fica responsável por pivotar um número de linhas de maneira crescente ao seu ID.
*/
void merge_matrix (const int world_rank, const int world_size, Data *data) {
    size_t size = line(data)*col(data);
    float *vector = malloc(sizeof(float) * size);
    
    if (NULL != vector) {
        /*  Uma  estrutura  de  anel para passar as linhas do processo anterior que já foram pivotadas com a do processo
            atual e passar para o próximo processo. */
        if (is_root(world_rank)) {
            matrix_to_vector(data, vector);
            MPI_Send(vector, size, MPI_FLOAT, world_rank+1, 0, MPI_COMM_WORLD);
        } else if (!is_tail(world_rank, world_size)) {
            MPI_Recv(vector, size, MPI_FLOAT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            /*  Juntar a matrix com as linhas pivotadas até este processo com as pivotadas por este processo.    */
            merge_pivoting(world_rank, world_size, data, vector);
            MPI_Send(vector, size, MPI_FLOAT, world_rank+1, 0, MPI_COMM_WORLD);
        } else {
            /*  Quando  o  processo  for  o  tail,  ele  apenas  juntará  toda a informação em uma matriz final que será
                utilizada posteriormente para zerar as colunas.   */
            MPI_Recv(vector, size, MPI_FLOAT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            merge_pivoting(world_rank, world_size, data, vector);
            vector_to_matrix(data, vector);
        }

        free(vector);
    }
}

/*
*   Dada a matriz já pivotada, se zera as colunas desses pivots.
*/
void clear_columns (Data *data) {
    size_t i = 0, j = 0, k = 0;
    float pivot = 0, factor = 0;
    
    if (NULL != data) {
        /*  Uma linha de cada vez da matrix será selecionada para zerar a coluna do seu pivot nas outras linhas.    */
        for (; i < line(data); i++) {
            pivot = matrix(data)[i][i];

            /*  O pivot será zero quando alguma chamada anterior acabou por zerar a sua posição.    */
            if (0 != pivot) {
                /*  Seleciona-se todas as outras linhas da matriz para zerar a coluna do pivot. */
                for (j = 0; j < line(data); j++) {
                    /*  Não faz sentido procurar zerar a coluna na linha do próprio pivot.  */
                    if (i != j) {
                        /*  Dado o valor da coluna na outra linha, se cacula qual o coeficiente para multiplicar a linha
                            do pivot para subtrair na linha em que se procura zerar a coluna.   */
                        factor = matrix(data)[j][i]/pivot;
    
                        /*  Na linha que se busca zerar a coluna, subtrair a linha do pivot.    */
                        #pragma omp parallel for
                        for (k = 0; k < col(data); k++) {
                            matrix(data)[j][k] -= factor*matrix(data)[i][k];
                        }
                    }
                }
            }
        }

        /*  Como  após  zerar-se  as  colunas  os pivots podem mudar de valor, se divide as linhas pelos valores de seus
            pivots. */
        for (i = 0; i < line(data); i++) {
            pivot = matrix(data)[i][i];

            #pragma omp parallel for
            for (j = 0; j < col(data); j++) {
                matrix(data)[i][j] /= pivot;
            }
        }
    }
}

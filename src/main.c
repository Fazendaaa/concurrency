/*

*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

/*  ---------------------------------------------------- STRUCTS ---------------------------------------------------  */

typedef struct data {
    unsigned int start_line, chunk, exced;
} Data;

/*  ---------------------------------------------------- DEFINES ---------------------------------------------------  */

#define is_root(m)      ((0 == m) ? 1 : 0)
#define is_tail(p, w)   ((p == w-1) ? 1 : 0)

#define start(d)        (d.start_line)
#define chunk(d)        (d.chunk)
#define exced(d)        (d.exced)

/*  ---------------------------------------------------- HEADERS ---------------------------------------------------  */

static int ** matrix_init (const unsigned int);
static void print_matrix (int **, const unsigned int);
static void free_matrix (int ***, const unsigned int);

static void ring (int, char**, int **, const unsigned int);
static void threads (int **, const unsigned int, Data, const unsigned int);

/*  ----------------------------------------------------- MAIN -----------------------------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    unsigned int matrix_size = 4;
    int **matrix = matrix_init(matrix_size);
   
    ring (argc, argv, matrix, matrix_size);
    free_matrix(&matrix, matrix_size);

    return 0;
}

/*  ------------------------------------------------ IMPLEMENTATIONS -----------------------------------------------  */

static int ** matrix_init (const unsigned int len) {
    int **matrix = (int**) malloc(sizeof(int *) * len);
    unsigned int i, j;

    if (NULL != matrix) {
        for (i = 0; i < len; i++) {
            matrix[i] = (int*) malloc(sizeof(int) * (len+1));
            
            for (j = 0; j < len+1; j++)
                matrix[i][j] = i+1;
        }
    }

    return matrix;
}

static void print_matrix (int ** matrix, const unsigned int len) {
    unsigned int i, j;

    if (NULL != matrix) {
        for (i = 0; i < len; i++) {
            for (j = 0; j < len+1; j++) 
                printf("%d ", matrix[i][j]);
            printf("\n");
        }
        printf("\n");
    }
}

static void free_matrix (int *** matrix, const unsigned int len) {
    unsigned int i;

    if (NULL != matrix) {
        for (i = 0; i < len; i++) 
            free((*matrix)[i]);
        free(*matrix);
        *matrix = NULL;
    }
}

static void ring (int argc, char **argv, int **matrix, const unsigned int matrix_size) {
    unsigned int i = 0, j = 0, excess = 0;
    int tag = 0, pivot = 0, pivot_pos = 0;
    Data data;
    int world_size = 0, world_rank = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (is_root(world_rank)) {
        /*  Caso a matrix nao consiga ser dividida igualmente entre os processos, o ultimo arcara com o excesso.    */
        exced(data) = matrix_size%world_size;
        chunk(data) = matrix_size/world_size;
        start(data) = 0;
    } else {
        /*  Recebe o dado anterior enviado pelo anel    */
        MPI_Recv(&data, sizeof(Data), MPI_BYTE, world_rank-1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    printf("[Process %d]\nstart: %d\nchunck: %d\nexced: %d\n\n", world_rank, start(data), chunk(data), exced(data));
    /*  Caso seja o último processo, pegar o excesso caso exista.   */
    excess = (is_tail(world_rank, world_size)) ? 0 : exced(data);

    for (i = 0; i < chunk(data)+excess; i++) {
        pivot_pos = start(data)+i;
        pivot = matrix[pivot_pos][pivot_pos];

        printf("Pivot: %d\n", pivot);
        /*  Divide a linha do pivo pelo seu valor.  */
        for (j = 0; j < matrix_size+1; j++)
            matrix[pivot_pos][j] /= pivot;
    }

    threads(matrix, matrix_size, data, is_tail(world_rank, world_size));
    print_matrix(matrix, matrix_size);
    
    /*  atualiza qual vai ser a posicao inicial do próximo bloco a realizar as operacoes.   */
    start(data) += chunk(data);        
    MPI_Send(&data, sizeof(Data), MPI_BYTE, (world_rank+1)%world_size, tag, MPI_COMM_WORLD);

    /*  Caso seja o ultimo processo do anel.    */
    if (is_tail(world_rank, world_size)) {
        printf("Printing matrix after all calculos:\n");
        print_matrix(matrix, matrix_size);
    }

    MPI_Finalize();
}

static void threads (int **matrix, const unsigned int matrix_size, Data data, const unsigned int tail) {
    unsigned int i = 0, j = 0, k = 0, excess = 0;
    int pivot = 0, pivot_pos = 0;
    float mult = 0;
    
    /*  Parte para zerar a coluna do pivo nas outras linhas.    */
    #pragma omp parallel default(shared) private(pivot, pivot_pos)
    {    
        excess = (tail) ? 0 : exced(data);
        
        for (i = 0; i < chunk(data)+excess; i++) {
            pivot_pos = start(data)+i;
            pivot = matrix[pivot_pos][pivot_pos];

            /*  pega as outras linhas sem ser a do pivot e zera a coluna.   */
            for (j = 0; j < matrix_size; j++) {
                mult = (float)matrix[j][pivot_pos]/(float)pivot;
                
                for (k = 0; k < matrix_size+1; k++)
                    matrix[i][j] += (-mult) * matrix[j][pivot_pos];
            }
        }
    }
}

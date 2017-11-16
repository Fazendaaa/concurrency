/*
*   GJ -- Gauss-Jordan
*/
#ifndef GJ_H_
#define GJ_H_

#include "matrix.h"
#include <mpi.h>
#include <omp.h>

/*  ------------------------------------------------- DEFINES ------------------------------------------------------  */

#define is_root(p)      ((0 == p) ? 1 : 0)
#define is_tail(p, w)   ((w-1 == p) ? 1 : 0)

/*  ------------------------------------------------- HEADERS ------------------------------------------------------  */

/* Verifica se algum pivô é zero, e se for, executa a troca de linha */
void swapping (const int, const int, Data *, int order[]);
/* faz broadcast da matriz organizada para os processos não root */
void send_swap (const int world_rank, const int world_size, Data *data);
/*  Divide a matriz pelo valor do pivot de cada respectiva linha.   */
void pivoting (const int, const int, Data *);
/*  Junta as matrizes pivotadas por cada processo em uma só no processo de maior id.    */
void merge_matrix (const int, const int, Data *);
/*  Zera as colunas dos pivots. */
void clear_columns (Data *);

void write_result(Data *data, const int world_rank, int *order);

#endif

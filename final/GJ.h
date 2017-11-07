/*
*   GJ -- Gauss-Jordan
*/
#ifndef GJ_H_
#define GJ_H_

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

/*  ------------------------------------------------- DEFINES ------------------------------------------------------  */

#define is_root(p)      ((0 == p) ? 1 : 0)
#define is_tail(p, w)   ((w-1 == p) ? 1 : 0)

/*  ------------------------------------------------- HEADERS ------------------------------------------------------  */

/*  Divide a matriz pelo valor do pivot de cada respectiva linha.   */
void pivoting (const int, const int, int**, const size_t);
/*  Junta as matrizes pivotadas por cada processo em uma só no processo de maior id.    */
void merge_matrix (const int, const int, int**, const size_t);
/*  Zera as colunas dos pivots. */
void clear_columns (int**, const size_t);

#endif

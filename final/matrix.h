#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdio.h>
#include <stdlib.h>

/*  ------------------------------------------------- STRUCTS ------------------------------------------------------  */

typedef struct _ {
    float **matrix;
    size_t col, line;
} Data;

/*  ------------------------------------------------- DEFINES ------------------------------------------------------  */

#define matrix(d)   ((d)->matrix)
#define col(d)      ((d)->col)
#define line(d)     ((d)->line)

/*  ------------------------------------------------- HEADERS ------------------------------------------------------  */

/*  Lê os arquivos que conteem a matrix.    */
Data * matrix_read (const char *, const char *);
/*  Imprime a matrix.   */
void print_matrix (const Data *);
/*  Libera o espaço alocado da matrix.  */
void free_matrix (Data **);

#endif

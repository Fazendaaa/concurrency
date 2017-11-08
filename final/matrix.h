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

Data * matrix_read (const char *, const char *);
void print_matrix (const Data *);
void free_matrix (Data **);

#endif

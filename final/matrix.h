#ifndef MATRIX_H_
#define MATRIX_H_
#include "utils.h"

float **createMatrix(int size);
void freeMatrix(float ***tgt,int size);
void printMatrix(float **tgt,int size);
void adjustMatrix(float **tgt, int size);

#endif

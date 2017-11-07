#include "matrix.h"

float **createMatrix(int size){
    if(size<=0) return NULL;
    float **rt=NULL;
    int i;

    rt=(float **)malloc(size*sizeof(float));
    for(i=0;i<size;i++) rt[i]=(float *)malloc(size*sizeof(float));
    return rt;
}

void freeMatrix(float ***tgt,int size){
    if(tgt==NULL || *tgt==NULL || **tgt==NULL || size<=0) return;
    int i;

    for(i=0;i<size;i++) free(*(tgt[i]));
    free(*tgt);
    free(tgt);

}

void printMatrix(float **tgt,int size){
    int i,j;
    if(tgt==NULL || *tgt==NULL || size<=0) return;
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            printf("%f ",tgt[i][j]);
        }
        printf("\n");
    }
}

void swap(float **A, float **B,int size){
    if(A==NULL || B==NULL || *A==NULL || *B==NULL || size<=0) return;
    int i;
    float tmp;
    for(i=0;i<size;i++){
        tmp=*A[i];
        *A[i]=*B[i];
        *B[i]=tmp;
    }
}


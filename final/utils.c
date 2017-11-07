#include "utils.h"
#define ENTER '\n'
#define SPACE 32

void readFloat(FILE *src, float *rt,char *check){
    if(src==NULL) return;
    char *tmp=NULL,chk;
    int i;
    while(chk!=ENTER && chk !=SPACE){
        fread(&chk,sizeof(char),1,src);
        *check=chk;
        tmp=(char *)realloc(tmp,(i+1)*sizeof(char));
        tmp[i++]=chk;
    }
    tmp[i]='\0';
    *rt=atof(tmp);
    free(tmp);
}

float *readVector(FILE *src, int *vectorSize){
    if(src==NULL) return NULL;
    float *rt=NULL,tmp;
    char check='a';
    int i=0; 

    while(check!=ENTER){
        readFloat(src,&tmp,&check);
        rt=(float *)realloc(rt,(i+1)*sizeof(float));
        rt[i++]=tmp;
    }
    *vectorSize=i;
    rt[i]='\0';
    return rt;
}
void printVector(float *tgt,int size){
    int i;
    for(i=0;i<size;i++){
        printf("%.2f",tgt[i]);
        printf("\n");
    }
}
void readMatrix(float ***tgt,int size){
    int i=0, j=0;
    float tmp;
    char chk;
    FILE *src;
    
    if(tgt==NULL || size<=0) return;
    src=fopen("matrix.txt","r");
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            readFloat(src,&tmp,&chk);
            fread(&chk,sizeof(char),1,src);
            *(tgt[i][j])=tmp;
        }
    }
    fclose(src);
}

float *readSolution(){
    int size,i;
    FILE *src=fopen("vetor.txt","r");
    float *rt=NULL,tmp;
    char leftover;
    while(!feof(src)){
        readFloat(src, &tmp,&leftover);
        rt=(float *)realloc(rt,(i+1)*sizeof(float));
        rt[i++]=tmp;
    }
    rt[i]='\0';
    fclose(src);
    return rt;
}

void writeSolution(float *solution, int size){
    if(solution==NULL || size <=0) return;
    int i;
    FILE *answer=fopen("resultado.txt","w");

    for(i=0;i<size;i++)
        fprintf(answer,"%f\n",solution[i]);

    fclose(answer);
}

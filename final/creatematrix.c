#include <math.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **arqv){
    FILE *matrix_file, *result_file;
    float number;
    time_t t;

    srand((unsigned) time(&t));

    int size;

    scanf("%d", &size);

    matrix_file = fopen("matriz.txt", "wb");
    result_file = fopen("vetor.txt", "wb");

    int j = 0;
    int col = 0;

    for(int i = 0; i < size * size ; i++){

      //number = 1.0 * (rand() % 100);

      if(j == col){
        //fprintf(matrix_file, "%f", number);
        number = 1.0;
        fprintf(matrix_file, "%f", number);

      }else{
        number = 0.0;
        fprintf(matrix_file, "%f", number);
      }

      if( j == size - 1){
        fprintf(matrix_file, "\n");
        j = 0;
        col++;
      }else{
        fprintf(matrix_file, " ");
        j++;
      }
    }

    for(int i = 0; i < size; i++){
      number = 1.0 * (rand() % 100);
      fprintf(result_file, "%f", number);
      fprintf(result_file, "\n");
    }

    fclose(matrix_file);
    fclose(result_file);
}

#include "matrix.h"
#include <math.h>

/*  --------------------------------------------- IMPLEMENTATIONS --------------------------------------------------  */

/*
*   Pega os valores lidos do arquivo e retorna-os para a matrix em memória.
*/
static float ** file_to_matrix (const float * line, const float * result, const size_t n_lines){
    size_t i = 0, j = 0, k = 0, l = 0, n_cols = 0;
    float **matrix = NULL;

    if (NULL != line && NULL != result && 0 < n_lines) {
        matrix = (float **) malloc(sizeof(float*) * n_lines);
        n_cols = n_lines + 1;
        
        if (NULL != matrix) {
            for (; i < n_lines; i++) {
                matrix[i] = malloc(sizeof(float) * n_cols);

                if (NULL != matrix[i]) {
                    /*  Adiciona os valores lidos da matrix.    */
                    for (j = 0; j < n_lines; j++) {
                        matrix[i][j] = line[k++];
                    }

                    /*  Adiciona os valores lidos do resultado. */
                    matrix[i][j] = result[l++];
                }
            }
        }
    }

    return matrix;
}

/*
*   Lê os arquivos e retorna a matrix.
*/  
Data * matrix_read (const char * matrix_name, const char * result_name) {
    Data *data = (Data*) malloc(sizeof(Data));
    size_t counter = 0, n_lines = 0;
    FILE *matrix_file = NULL, *result_file = NULL;
    float tmp, *line = NULL, *result = NULL;

    if (NULL != matrix_name && NULL != result_name && NULL != data) {
        matrix_file = fopen(matrix_name, "r");
        result_file = fopen(result_name, "r");

        if (NULL != matrix_file && NULL != result_file) {
            fseek(matrix_file, 0, 0);
            fseek(result_file, 0, 0);

            /*  Lê as linhas da matrix. */
            while (EOF != fscanf(matrix_file, "%f", &tmp)) {
                line = (float*) realloc(line, sizeof(float) * (counter+1));
                line[counter] = tmp;
                counter++;
            }
            
            /*  Já  que  todas  as  matrizes  são  quadradas,  basta  pegar o número de elementos lidos e extrair a raiz
                quadrada para se chegar ao número de linhas.    */
            n_lines = sqrt(counter);

            /*  Como o número de linhas lidas da matriz é igual ao número de linhas do resultado, se pode alocar já aqui
                mesmo  a  memória necessária, sem a necessidade de ficar fazendo reallocs posteriormente de acordo com o
                input lido. */
            result = (float*) malloc(sizeof(float) * n_lines);
            
            counter = 0;
            if (NULL != result) {
                /*  Lê as linhas do resultado.  */
                while (EOF != fscanf(result_file, "%f", &tmp)) {
                    result[counter] = tmp;
                    counter++;
                }
            }

            /*  Uma vez com a matriz e o resultado lido, se passa esses valores para a matriz que será utilizada.   */
            matrix(data) = file_to_matrix(line, result, n_lines);

            line(data) = n_lines;
            col(data) = n_lines + 1;

            fclose(matrix_file);
            fclose(result_file);
        }
    }

    return data;
}

/*
*   Imprime uma matrix do tipo NxN+1.
*/
void print_matrix (const Data * data) {
    size_t i = 0, j = 0;

    if (NULL != data) {
        for (; i < line(data); i++) {
            for (j = 0; j < col(data); j++) {
                printf("%f ", matrix(data)[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

/*
*   Libera o espaço alocado da matrix da mememória.
*/
void free_matrix (Data ** data) {
    size_t i = 0;
    
    if (NULL != data && NULL != *data) {
        for (; i < line(*data); i++) {
            free(matrix(*data)[i]);
        }
        free(matrix(*data));
        free(*data);
        *data = NULL;
    }
}

#include "matrix.h"

/*  --------------------------------------------- IMPLEMENTATIONS --------------------------------------------------  */

/*
*   Pega uma linha de floats em char e a passa para floats.
*/
static float * line_to_vector (const char *buffer_matrix, const char *buffer_result) {
    size_t i = 0, j = 0, k = 0;
    char temp[10];
    float *line = NULL;

    /*  Adiciona os valores da matrix.  */
    while('\n' != buffer_matrix[i] && '\0' != buffer_matrix[i]) {
        /*  Adiciona um número em uma variável temporária até ler um espaço.    */
        if (' ' != buffer_matrix[i]) {
            temp[j++] = buffer_matrix[i];
        /*  Adiciona mais um espaço na matrix e adiciona o último número lido.  */
        } else {
            line = realloc(line, sizeof(float) * (k+1));
            line[k++] = atof(temp);
            j = 0;
        }

        i++;
    }

    /*  Adiciona o resultado da linha do sistema linear.    */
    line = realloc(line, sizeof(float) * (k+1));
    line[k] = atof(buffer_result);

    return line;
}

/*
*   Lê os arquivos e retorna a matrix.
*/  
Data * matrix_read (const char * matrix_name, const char * result_name) {
    Data *data = malloc(sizeof(Data));
    size_t matrix_size = sizeof(char) * 125, result_size = sizeof(char) * 10, counter = 0;
    char *buffer_matrix = malloc(matrix_size), *buffer_result = malloc(result_size);
    FILE *matrix_file = NULL, *result_file = NULL;

    if (NULL != matrix_name && NULL != result_name && NULL != data) {
        matrix_file = fopen(matrix_name, "r");
        result_file = fopen(result_name, "r");

        if (NULL != matrix_file && NULL != result_file) {
            fseek(matrix_file, 0, 0);
            fseek(result_file, 0, 0);

            /*  Lê as linhas da matrix. */
            while (NULL != fgets(buffer_matrix, matrix_size, matrix_file) &&
                   NULL != fgets(buffer_result, result_size, result_file)) {
                /*  Adiciona mais uma linha na matrix.  */
                matrix(data) = realloc(matrix(data), sizeof(float*) * (counter+1));

                /*  Lê o conteúdo da linha lida e transforma ela em array de floats.    */
                matrix(data)[counter] = line_to_vector(buffer_matrix, buffer_result);
                counter++;
            }
            
            line(data) = counter;
            col(data) = counter + 1;

            fclose(matrix_file);
            fclose(result_file);
        }
    }

    free(buffer_matrix);
    free(buffer_result);

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

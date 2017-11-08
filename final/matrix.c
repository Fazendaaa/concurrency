#include "matrix.h"

/*  --------------------------------------------- IMPLEMENTATIONS --------------------------------------------------  */

/*
*   Pega uma linha de floats em char e a passa para floats.
*/
static float * line_to_vector (char *buffer) {
    size_t i = 0;
    float *line = NULL;

    while(' ' != buffer[i] && '\n' != buffer[i] && '\0' != buffer[i]) {
        printf("%c ",  buffer[i]);
        i++;
    }
    printf("\n");

    return line;
}

/*
*   Apenas inicializa uma matriz com dummy data.
*/  
Data * matrix_read (const char * matrix_name, const char * result_name) {
    size_t buffer_size = sizeof(char) * 125, counter = 0;
    char *buffer = malloc(buffer_size);
    Data *data = malloc(sizeof(Data));
    FILE *matrix_file = fopen(matrix_name, "r"), *result_file = fopen(matrix_name, "r");

    if (NULL != matrix_file && NULL != result_file && NULL != data) {
        /*  Lê as linhas da matrix. */
        while (NULL != fgets(buffer, buffer_size, matrix_file)) {
            /*  Adiciona mais uma linha na matrix.  */
            // matrix(data) = realloc(matrix(data), sizeof(float*)*(counter+1));

            /*  Lê o conteúdo da linha lida e transforma ela em array de floats */
            // matrix(data)[counter++] = line_to_vector(buffer);
            line_to_vector(buffer);
        }
        
        line(data) = counter;
        col(data) = counter;

        fclose(matrix_file);
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

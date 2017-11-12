/*
    Cuidado ao verificar as questões de divizões por quando o pivot for igual a zero.
*/
#include <time.h>
#include "matrix.h"
#include "GJ.h"


/*  -------------------------------------------------- MAIN --------------------------------------------------------  */

int main (int argc, char **argv) {
    /*  Variáveis do programa.  */
    Data *data = NULL;
    int *order = NULL;
    /*  Variáveis MPI.  */
    int world_size = 0, world_rank = 0;
    double total_exec_time_start = 0.0, total_exec_time_end = 0.0;
    clock_t start, end;
    double cpu_time_used;

    MPI_Init(&argc, &argv);

    start = clock();
    total_exec_time_start = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    data = matrix_read("matriz.txt", "vetor.txt");

    order = malloc(sizeof(int) * line(data));

    for(int i = 0; i < line(data); i++) order[i] = -1;

    if (is_root(world_rank)) {
        //printf("\nORIGINAL MATRIX:\n");
        //print_matrix(data);
    }
    printf("ready for swap\n");
    swapping(world_rank, world_size, data, order);

    MPI_Barrier(MPI_COMM_WORLD);

    printf("swapped\n");

    send_swap(world_rank, world_size, data);

    MPI_Barrier(MPI_COMM_WORLD);

    printf("send swap\n");

    if (is_root(world_rank)) {
        //for(int i = 0; i < line(data); i++) printf("%d\n", order[i]);
        //printf("\nORIGINAL MATRIX:\n");
        //print_matrix(data);
    }

    printf("pivoting\n");

    pivoting(world_rank, world_size, data);

    /*printf("#%d PROCESS\n", world_rank);
    print_matrix(data);*/

    printf("merged\n");

    merge_matrix(world_rank, world_size, data);

    if (is_tail(world_rank, world_size)) {
        //printf("\nBEFORE CLEANING COLLUMNS:\n");
        //print_matrix(data);
        printf("entrou no if antes de dar clear\n");

        clear_columns(data);

        printf("cleared\n");

        //printf("\nFINAL MATRIX:\n");
        //print_matrix(data);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printf("writting result\n");

    write_result(data, world_rank, order);

    MPI_Barrier(MPI_COMM_WORLD);

    end = clock();

    total_exec_time_end = MPI_Wtime();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tempo de execução total: %f\n", total_exec_time_end - total_exec_time_start);

    printf("Tempo de execução total pelo time.h: %f\n", cpu_time_used);

    MPI_Finalize();

    free_matrix(&data);

    return 0;
}

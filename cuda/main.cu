/*  Example from "Introduction to CUDA C" from NVIDIA website:
        https://developer.nvidia.com/cuda-education
   
    Compile with:
        $ nvcc example_intro.cu
*/
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

const int side = 16;
const int N = side*side;
const int THREADS_PER_BLOCK = N;

/* While doing this seems unecessary, in some cases we need threads
   since they have communication (__shared__ variables) and
   synchronization (__syncthreads()) mechanisms */
__global__ void mult_threads_blocks (int *a, int *b, int *c, int n) {
    /*  blockDim.x gives the number of threads per block, combining it
        with threadIdx.x and blockIdx.x gives the index of each global
        thread in the device */
    int index = threadIdx.x * blockIdx.x * threadIdx.x;

    /*  Typical problems are not friendly multiples of blockDim.x.
        Avoid accesing data beyond the end of the arrays    */
    if (index < n) {
        c[index] += a[index] * b[index];
    }
}

int main(void) {
    /*  Host (CPU) copies of a, b, c    */
    int *a = NULL, *b = NULL, *c = NULL;
    /*  Device (GPU) copies of a, b, c  */
    int *d_a = NULL, *d_b = NULL, *d_c = NULL;
    /*  Data size   */
    size_t size = N*sizeof(int);

    /*  Allocate memory in host */
    a = (int *) malloc(size);
    b = (int *) malloc(size);
    c = (int *) malloc(size);

    /*  Allocate memory in device   */
    cudaMalloc((void **)&d_a, size);
    cudaMalloc((void **)&d_b, size);
    cudaMalloc((void **)&d_c, size);

    /*  Allocate data in vectors a and b (inside host)  */
    for (int i = 0; i < N; i++) {
        a[i] = i;
        b[i] = i;
        c[i] = 0;
    }

    /*  Copy data to device */
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_c, c, size, cudaMemcpyHostToDevice);

    /*  Launch mult() kernel on device with N threads in N blocks   */
    mult_threads_blocks<<<(N + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(d_a, d_b, d_c, N);
    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);

    printf("[RESULT MATRIX]\n");
    for (int i = 0; i < side; i++) {
        for (int j = i*side; j < (i+1)*side; j++) {
            printf("%d ", c[j]);
        }
        printf("\n");
    }

    /*  Clean-up    */
    free(a);
    free(b);
    free(c);
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    return 0;
}

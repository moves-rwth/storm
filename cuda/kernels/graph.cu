#include <stdio.h>
 
const int N = 16; 
const int blocksize = 16;

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

__global__ 
void hello(char *a, int *b) 
{
    a[threadIdx.x] += b[threadIdx.x];
}

namespace stormcuda {
    namespace graph {
        void helloWorld() {
            printf("CUDA TEST START\n");
            printf("Should print \"Hello World\"\n");

            char a[N] = "Hello \0\0\0\0\0\0";
            int b[N] = {15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            char c[N] = "YELLO \0\0\0\0\0\0";

            char *ad;
            int *bd;
            const int csize = N * sizeof(char);
            const int isize = N * sizeof(int);

            printf("%s", a);

            cudaMalloc((void **) &ad, csize);
            cudaMalloc((void **) &bd, isize);
            cudaMemcpy(ad, a, csize, cudaMemcpyHostToDevice);
            cudaMemcpy(bd, b, isize, cudaMemcpyHostToDevice);

            dim3 dimBlock(blocksize, 1);
            dim3 dimGrid(1, 1);
            hello << < dimGrid, dimBlock >> > (ad, bd);

            gpuErrchk(cudaPeekAtLastError());
            gpuErrchk(cudaDeviceSynchronize());

            cudaMemcpy(c, ad, csize, cudaMemcpyDeviceToHost);
            cudaFree(ad);
            cudaFree(bd);

            printf("%s\n", c);
            printf("CUDA TEST END\n");
        }
    }
}

#include <immintrin.h>
#include <stdlib.h>
#include <iostream>
#include <cstdint>
#include <string.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <unistd.h>

#include "hpc_helpers.h"

void naiveMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L);
void transposeAndMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L);
void avxTransposeAndMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L);
void blockedMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L, uint64_t blkSz);
void avxBlockedMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L, uint64_t blkSz);
void initialize(float * array, uint64_t size);
void compare(float * array1, float * array2, uint64_t size);
void cacheFlush();

#define TESTS 4
int main () 
{
                                //M,     N,      L,      blkSz 
    uint64_t sizes[TESTS][4] = {{1 << 9, 1 << 9, 1 << 5, 16},
                                {1 << 10, 1 << 10, 1 << 5, 16},
                                {1 << 10, 1 << 10, 1 << 6, 32},
                                {1 << 11, 1 << 11, 1 << 6, 32}};

    for (int i = 0; i < TESTS; i++)
    {
        uint64_t M = sizes[i][0];
        uint64_t N = sizes[i][1];
        uint64_t L = sizes[i][2];
        uint64_t blkSz = sizes[i][3];
        if ((blkSz % 8) != 0)
        {
            printf("Error: block size %d is not a multiple of 8\n", blkSz);
            exit(1);
        }

        printf("\n%d by %d TIMES %d by %d EQUALS %d by %d\n", M, L, L, N, M, N);
        printf("BLOCKSIZE EQUALS %d\n", blkSz);

        float * A = (float *)aligned_alloc(32, sizeof(float) * M * L);
        float * B = (float *)aligned_alloc(32, sizeof(float) * L * N);    //L rows, N columns
        float * Cn = new float[M * N];                                    //naive multiply
        float * Ct = new float[M * N];                                    //transpose multiply
        float * Cat = (float *)aligned_alloc(32, sizeof(float) * M * N);  //avx transpose and multiply
        float * Cb = new float[M * N];                                    //blocked multiply
        float * Cab = (float *)aligned_alloc(32, sizeof(float) * M * N);  //avx blocked multiply
        initialize(A, M * L);
        initialize(B, L * N);

        //perform matrix multiply the naive way to check for correctness
        cacheFlush();
        TIMERSTART(naive_mult)
        naiveMult(A, B, Cn, M, N, L);
        TIMERSTOP(naive_mult)

        cacheFlush();
        TIMERSTART(transpose_and_mult)
        transposeAndMult(A, B, Ct, M, N, L);
        TIMERSTOP(transpose_and_mult)
        compare(Cn, Ct, M * N);

        cacheFlush();
        TIMERSTART(avx_transpose_and_mult)
        avxTransposeAndMult(A, B, Cat, M, N, L);
        TIMERSTOP(avx_transpose_and_mult)
        compare(Cn, Cat, M * N);
        //output speedup of the avxTransposeAndMult over transposeAndMult
        //copy your SPEEDUP macro from assignment 1 into the .h file and uncomment this
        //SPEEDUP(avx_transpose_and_mult, transpose_and_mult)

        cacheFlush();
        TIMERSTART(blocked_mult)
        blockedMult(A, B, Cb, M, N, L, blkSz);
        TIMERSTOP(blocked_mult)
        //make sure that the results of the blockedMult are the same as naiveMult
        compare(Cn, Cb, M * N);

        cacheFlush();
        TIMERSTART(avx_blocked_mult)
        avxBlockedMult(A, B, Cab, M, N, L, blkSz);
        TIMERSTOP(avx_blocked_mult)
        //make sure that the results of the avxBlockedMult are the same as naiveMult
        compare(Cn, Cab, M * N);
        //output speedup of the avxBlockedMult over the blockedMult
        //copy your SPEEDUP macro from assignment 1 into the .h file and uncomment this
        //SPEEDUP(avx_blocked_mult, blocked_mult)

        delete A; 
        delete B; 
        delete Cn; 
        delete Ct; 
        delete Cat; 
        delete Cb; 
        delete Cab; 
    }
}

/* 
 * Initialize an array of size floats to random values between 0 and 9.
 */
void initialize(float * array, uint64_t size)
{
   for (uint64_t i = 0; i < size; i++) array[i] = rand() % 10;
}   

/* 
 * Compare two arrays.  Output a message and exit program if not the same. 
 */
void compare(float * array1, float * array2, uint64_t size)
{
    for (uint64_t i = 0; i < size; i++)
    {
        if (array1[i] != array2[i])
        {
            printf("Error: arrays do not match\n");
            printf("       index %d: %6.2f != %6.2f\n", i, array1[i], array2[i]);
            exit(1);
        }
    }

}

/* 
 * Perform a matrix multiply A * B and store the result in array C.
 * Use the naive matrix multiply technique.
 * A is of size M by L, 
 * B is of size L by N, 
 * C is of size M by N
 */
void naiveMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L)
{
    for (uint64_t i = 0; i < M; i++)
    {
        for (uint64_t j = 0; j < N; j++)
        {
            float accum = 0;
            for (uint64_t k = 0; k < L; k++)
            {
                accum += A[i*L+k]*B[k*N+j];
            }
            C[i*N+j] = accum;
        }
    }
}

/* 
 * Perform a matrix multiply A * B and store the result in array C.
 * Transpose the B array before doing the matrix multiply.
 * A is of size M by L, 
 * B is of size L by N, 
 * C is of size M by N
 */
void transposeAndMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L)
{
    float * Bt = new float[N*L];
    for (uint64_t k = 0; k < L; k++)
        for (uint64_t j = 0; j < N; j++)
            Bt[j*L+k] = B[k*N+j];

    for (uint64_t i = 0; i < M; i++)
    {
        for (uint64_t j = 0; j < N; j++)
        {
            float accum = 0;
            for (uint64_t k = 0; k < L; k++)
                accum += A[i*L+k]*Bt[j*L+k];
            C[i*N+j] = accum;
        }
    }
    delete Bt;
}

/* 
 * Perform a matrix multiply A * B and store the result in array C.
 * Transpose B before doing multiply.  Use AVX instructions to load
 * elements from A and Bt and perform the multiplication. 
 * A is of size M by L, 
 * B is of size L by N, 
 * C is of size M by N
 */
void avxTransposeAndMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L)
{

    /* Here is the transpose. */
    float * Bt = (float *)aligned_alloc(32, sizeof(float) * N * L);
    for (uint64_t k = 0; k < L; k++)
        for (uint64_t j = 0; j < N; j++)
            Bt[j*L+k] = B[k*N+j];

    /* You'll need to implement the matrix multiply. */
    /* You can use Listing 3.2 in the textbook as a resource, but that code uses */
    /* AVX2 instructions and our student2 machine only supports AVX instructions. */
    /* Instruction set described here: https://software.intel.com/sites/landingpage/IntrinsicsGuide/# */
    /* Click on AVX under the list of technologies. */
    for (uint64_t i = 0; i < M; i++)
    {
        for (uint64_t j = 0; j < N; j++) 
        {
            __m256 X = _mm256_setzero_ps();
            __m256 temp = _mm256_setzero_ps();
            for (uint64_t k = 0; k < L; k+=8) {
                const __m256 AV = _mm256_load_ps(A+i*L+k);
                const __m256 BV = _mm256_load_ps(Bt+j*L+k);
                //X = _mm256_fmadd_ps(AV, BV, X);
                temp = _mm256_mul_ps(AV, BV);
                X    = _mm256_add_ps(temp, X); 
            }
            float arr[8];

            float total = 0;
            _mm256_store_ps(arr, X);
            for (int k = 0; k < 8; k++) //Couldn't figure out the length of the array here, M, N and L were too short, M*N broke it. -A 
            {
                total += arr[k];
            }
            C[i*N+j]   = total;
            //C[i*N+j] = hsum_avx(X);

        }
    }
    delete Bt; 
}

/**
float sumArray(__m256 array, uint64_t length)
{
    float sum = 0;
    __m256 X  = _mm256_setzero_ps();
    for (uint64_t i = 0; i < length; i+=8)
    {
        __m256 AV = _mm256_load_ps(array+i);
        X = _mm256_add_ps(AV, X);
    }
    float arr[8];
    _mm256_store_ps(arr, X);
    for(int k = 0; k < 8; k++)
    {
        sum+=arr[k];
    }
    return sum;

}
**/

/* 
 * Perform a matrix multiply A * B and store the result in array.
 * Use the blocking matrix multiply technique:
 * https://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf
 * A is of size M by L, 
 * B is of size L by N, 
 * C is of size M by N
*/
void blockedMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L, uint64_t blkSz)
{
   //Use https://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf
   //as a reference.  However in that version A, B, and C are all n by n matrices.  Thus,
   //you'll need to make some changes to the code so that it works for these difference array sizes.
    //ROWS COME FIRST, ROW MAJOR !!!!!!!!!!!
   uint64_t i, j, k, kk, jj;
   float sum  = 0;
   //uint64_t eM = blkSz * (M/blkSz); //A 
   uint64_t eA = blkSz * (L/blkSz);
   uint64_t eB = blkSz * (N/blkSz); 
   
   for (i = 0; i < M; i++) {
      for (j = 0; j < N; j++) {
         C[i*N + j] = 0;
      }
   }

    for (kk = 0; kk < eA; kk += blkSz) {
        for (jj = 0; jj < eB; jj += blkSz) {
            for (i = 0; i < N; i++) {
                for (j = jj; j < jj + blkSz; j++) {
                sum = C[i*N + j];
                    for (k = kk; k < kk + blkSz; k++) {
                    sum += A[i*L + k]*B[k*N + j];
               }
               C[i*N + j] = sum;
            }
         }
      }
   }
} 

/* 
 * Perform a matrix multiply A * B and store the result in array.
 * Use the blocking matrix multiply technique:
 * https://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf
 * and use AVX instructions.
 * Transpose B before the blocking loop since AVX instructions don't
 * provide a load from non-consecutive memory locations.
 * A is of size M by L, 
 * B is of size L by N, 
 * C is of size M by N
*/
void avxBlockedMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L, 
                    uint64_t blkSz)
{
    /*
     * Before the blocking loop, transpose the array B.  We need to do that
     * because the _mm256_load_ps doesn't load non-consecutive elements.  (We call
     * this a strided access.) Caveat: there may be a avx instruction to do it, but
     * I couldn't find it.
     */

    float * Bt = (float *)aligned_alloc(32, sizeof(float) * N * L);
    for (uint64_t k = 0; k < L; k++)
        for (uint64_t j = 0; j < N; j++)
            Bt[j*L+k] = B[k*N+j];

    /* 
     * For the blocking code, you'll need to change the innermost loop so that it
     * loads 8 elements from A, 8 elements from transposed B, does the multiply, and
     * sums the products in the result vector to store in the C element.
     */ 
     
   uint64_t i, j, k, kk, jj;
   float sum  = 0;
   uint64_t eM = blkSz * (M/blkSz); 
   uint64_t eL = blkSz * (L/blkSz);

   for (i = 0; i < M; i++) {
      for (j = 0; j < N; j++) {
         C[i*N + j] = 0;
      }
   }
    __m256 X = _mm256_setzero_ps();
    __m256 AV = _mm256_load_ps(A);
    __m256 BV = _mm256_load_ps(B);
    __m256 CV = _mm256_load_ps(C);
    __m256 R  = _mm256_add_ps(AV, BV);
    float result[M];
    float total;

    for (kk = 0; kk < eM; kk += blkSz) {
       for (jj = 0; jj < eL; jj += blkSz) {
          for (i = 0; i < N; i++) {
             for (j = jj; j < jj + blkSz; j++) {
                sum = C[i*N + j];
                for (k = kk; k < kk + blkSz; k++) 
                {
                    __m256 AV = _mm256_load_ps(A+8);
                    __m256 BV = _mm256_load_ps(B+8);
                    __m256 R  = _mm256_mul_ps(AV, BV);
                }
                X    = _mm256_add_ps(R, X); 
                _mm256_store_ps(result, X);
                for (int k = 0; k < M; k++) //Couldn't figure out the length of the array here, M, N and L were too short, M*N broke it. -A 
                {
                    total += result[k];
                }
                C[i*N + j] = total;
            }
         }
      }
   }
}

/*
 * Call this function before the matrix multiply to flush the cache so that none of A or B are in the
 * cache before the multiply is performed.
 * I couldn't find a fool proof way to do this. cacheflush isn't available to us. 
 * But this seems to work.
 */
void cacheFlush()
{
    /* Going to sleep for a few seconds causes a context switch */
    sleep(3);
}

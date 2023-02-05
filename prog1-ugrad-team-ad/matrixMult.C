#include <iostream>
#include <cstdint>
#include <vector>
#include <string.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <unistd.h>

#include "hpc_helpers.h"

void naiveMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L);
void transposeAndMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L);
void blockedMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L, uint64_t blkSz);
void initialize(float * array, uint64_t size);
void compare(float * array1, float * array2, uint64_t size);
void cacheFlush();

/* You have three tasks:
 * I. Implement the blockedMult routine.
 * II. Add a SPEEDUP macro to hpc_helpers.h and uncomment the use of it in the main.
 * III. Answer the questions in the QUESTIONS file.
*/

#define TESTS 4
int main () 
{
                                //M,     N,      L,      blkSize 
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

        printf("\n%ld by %ld TIMES %ld by %ld EQUALS %ld by %ld\n", M, L, L, N, M, N);
        printf("BLOCKSIZE EQUALS %ld\n", blkSz);

        TIMERSTART(init)
        float * A = new float[M * L];  //M rows, L columns
        float * B = new float[L * N];   //L rows, N columns
        float * Cn = new float[M * N];  //naive multiply
        float * Ct = new float[M * N];  //transpose and multiply
        float * Cb = new float[M * N];  //blocked multiply
        initialize(A, M * L);
        initialize(B, L * N);
        TIMERSTOP(init)

        cacheFlush();
        TIMERSTART(naive_mult)
        naiveMult(A, B, Cn, M, N, L);
        TIMERSTOP(naive_mult)

        cacheFlush();
        TIMERSTART(transpose_and_mult)
        transposeAndMult(A, B, Ct, M, N, L);
        TIMERSTOP(transpose_and_mult)
        //make sure that the result of the transposeAndMult are the same as naiveMult
        compare(Cn, Ct, M * N);
        //output speedup of the transposeAndMult
        SPEEDUP(transpose_and_mult, naive_mult)

        cacheFlush();
        TIMERSTART(blocked_mult)
        blockedMult(A, B, Cb, M, N, L, blkSz);
        TIMERSTOP(blocked_mult)
        //make sure that the result of the blockedMult are the same as naiveMult
        compare(Cn, Cb, M * N);
        //output speedup of the blockedMult
        SPEEDUP(blocked_mult, naive_mult)

        delete A; 
        delete B; 
        delete Cn; 
        delete Ct; 
        delete Cb; 
    }
}

/* 
 * Initialize an array of size floats to random values between 0 and 9 .
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
            printf("       index %ld: %6.2f != %6.2f\n", i, array1[i], array2[i]);
            exit(1);
        }
    }
}

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
 * Transpose B before doing multiply.
 * A is of size M by L, 
 * B is of size L by N, 
 * C is of size M by N
 */
void transposeAndMult(float * A, float * B, float * C, uint64_t M, uint64_t N, uint64_t L)
{
    float * Bt = new float[N*L];
    TIMERSTART(transpose)
    for (uint64_t k = 0; k < L; k++)
        for (uint64_t j = 0; j < N; j++)
            Bt[j*L+k] = B[k*N+j];
    TIMERSTOP(transpose)

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

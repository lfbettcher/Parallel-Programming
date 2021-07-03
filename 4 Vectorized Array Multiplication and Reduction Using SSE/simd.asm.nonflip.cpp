#include "simd.p5.h"

// Modified ASM code by James Leflang
// https://edstem.org/us/courses/5332/discussion/421106?answer=994963

void SimdMul(float *a, float *b, float *c, int len) {
  int limit = (len / SSE_WIDTH) * SSE_WIDTH;

  asm("movq    %[a], %%r8\n\t"   // a
      "movq    %[b], %%rcx\n\t"  // b
      "movq    %[c], %%rdx\n\t"  // c
      :                          /* No Output */
      : [a] "r"(a), [b] "r"(b), [c] "r"(c));

  for (int i = 0; i < limit; i += SSE_WIDTH) {
    asm("movups    (%%r8), %%xmm0\n\t"   // load the first sse register
        "movups    (%%rcx), %%xmm1\n\t"  // load the second sse register
        "mulps    %%xmm1, %%xmm0\n\t"    // do the multiply
        "movups    %%xmm0, (%%rdx)\n\t"  // store the result
        "addq $16, %%r8\n\t"
        "addq $16, %%rcx\n\t"
        "addq $16, %%rdx\n\t"
        : /* No Output */
        : /* No Input */
    );
  }

  for (int i = limit; i < len; i++) {
    c[i] = a[i] * b[i];
  }
}

float SimdMulSum(float *a, float *b, int len) {
  float sum[4] = {0., 0., 0., 0.};
  int limit = (len / SSE_WIDTH) * SSE_WIDTH;

  asm("movq    %[a], %%r8\n\t"       // a
      "movq    %[b], %%rcx\n\t"      // b
      "leaq    %[sum], %0\n\t"       // &sum[0]
      "movups     (%0), %%xmm2\n\t"  // 4 copies of 0. in xmm2
      :                              /* No Output */
      : [a] "r"(a), [b] "r"(b), [sum] "m"(sum)
      : "%r8", "%rcx", "%xmm2");

  for (int i = 0; i < limit; i += SSE_WIDTH) {
    asm("movups    (%%r8), %%xmm0\n\t"   // load the first sse register
        "movups    (%%rcx), %%xmm1\n\t"  // load the second sse register
        "mulps    %%xmm1, %%xmm0\n\t"    // do the multiply
        "addps    %%xmm0, %%xmm2\n\t"    // do the add
        "addq $16, %%r8\n\t"
        "addq $16, %%rcx\n\t"
        : /* No Output */
        : /* No Input */
    );
  }

  asm("movups     %%xmm2, %[sum]\n\t"  // copy the sums back to sum[ ]
      : [sum] "=m"(sum)
      : /* No Input */
      : "memory");

  for (int i = limit; i < len; i++) {
    sum[0] += a[i] * b[i];
  }

  return sum[0] + sum[1] + sum[2] + sum[3];
}
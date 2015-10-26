#include "Fibonacci.h"

/**
 * Step function definition for "compute_fib"
 */
void Fibonacci_compute_fib(cncTag_t i, long *x, long *y, FibonacciCtx *ctx) {

    // Put "z" items
    long *z = (long *)cncItemAlloc(sizeof(*z));

    if (i <= ctx->n) {
      switch(i) {
        case 0: 
          *z = 0;
          break;
        case 1:
          *z = 1;
          break;
        default:
          *z = *x + *y;
      }

      cncPut_fib(z, i, ctx);
    }

}

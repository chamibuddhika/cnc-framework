#include "Fibonacci.h"


void Fibonacci_cncInitialize(FibonacciArgs *args, FibonacciCtx *ctx) {


    { // Prescribe "compute_fib" steps
        s64 _i;
        for (_i = 0; _i <= ctx->n; _i++) {
            cncPrescribe_compute_fib(_i, ctx);
        }
    }

    // Set finalizer function's tag
    Fibonacci_await(ctx);

}


void Fibonacci_cncFinalize(long *fib, FibonacciCtx *ctx) {

    /* TODO: Do something with fib */

  printf("Fibonacci %d : %ld\n", ctx->n, *fib);
}



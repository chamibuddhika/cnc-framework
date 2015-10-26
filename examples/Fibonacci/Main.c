#include "Fibonacci.h"

int cncMain(int argc, char *argv[]) {

    // Create a new graph context
    FibonacciCtx *context = Fibonacci_create();
    context->n = atoi(argv[1]);

    // TODO: Set up arguments for new graph initialization
    // Note that you should define the members of
    // this struct by editing Fibonacci_defs.h.
    FibonacciArgs *args = NULL;


    // TODO: initialize graph context parameters
    // int n;

    // Launch the graph for execution
    Fibonacci_launch(args, context);

    // Exit when the graph execution completes
    CNC_SHUTDOWN_ON_FINISH(context);

    return 0;
}

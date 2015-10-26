
// From https://engineering.purdue.edu/plcl/cnc2015/slides/cnc-framework.pdf

$context { int n; };

[ long *fib: i ];

( $initialize: () ) -> ( compute_fib: $rangeTo(0,#n) );

( compute_fib: i )
  <- [ x @ fib: i-2 ] $when(i>1),
     [ y @ fib: i-1 ] $when(i>1)
  -> [ z @ fib: i ];

( $finalize: () ) <- [ fib: #n ];

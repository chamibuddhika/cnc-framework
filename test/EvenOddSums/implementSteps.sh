#!/bin/bash

sed -i '{
s`/. TODO: Initialize naturals ./`*naturals = _i;`;
s`/. TODO: Do something with evensTotal ./`printf("Even total = %d\\n", evensTotal);`;
s`/. TODO: Do something with oddsTotal ./`printf("Odd total = %d\\n", oddsTotal);`;
s`/.TODO./0`i*2 + 1`;
}' EvenOddSums.c

sed -i '{
s`\(.*\)\(EvenOddSumsArgs.*=\)`\1\2 cncItemAlloc(sizeof(*args));\n\1args->n = 20;`;
}' Main.c

sed -i '{
s`.*{ // Access "evens" inputs$`    int sum = 0;\n&.`;
s`/. TODO: Do something with \([^ ]*\) ./`sum += \1;`;
s`/. TODO: Initialize \([^ ]*\) ./`*\1 = sum;`;
}' EvenOddSums_sumEvens.c

sed -i '{
s`.*{ // Access "odds" inputs$`    int sum = 0;\n&.`;
s`/. TODO: Do something with \([^ ]*\) ./`sum += \1;`;
s`/. TODO: Initialize \([^ ]*\) ./`*\1 = sum;`;
}' EvenOddSums_sumOdds.c

sed -i '{
s`/. TODO: Add struct members.`int n;`;
/^ *\*/d
}' EvenOddSums_defs.h


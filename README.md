# C Macro Programming Library

A complete primitive-recursive programming library for the C preprocessor with goodies.
It can be used for compile-time programming for C.

The C preprocessor language is a language of primitive recursive functions. This means, C preprocessor macros principally do not allow for unbounded computations; they are bounded. Recursions require distintictive name suffixes as the preprocessor does not allow the same name to be called directly or indirectly recursively.

The way, it is handled here, is to have a maximum number of loop cycles available after which the computation ends. You can add new cycles in `macro_loops.h`. The number of loop cycles is exponential. Therefore in practice, you often won't notice a difference with bounded computations. You probably will run out of space before you run out of loop cycles.

## Contents

It contains higher-order functions such as MAP, FILTER, REDUCE besides control structures and crazy stuff on top of it such as a FLATTEN which removes parenthesis pairs from the variadic arguments.

Finally, debugging macros are included which expand each loop cycle with index. You can look at them with the compiler command line option `-E` or expand macros in your IDE by hovering the mouse over them.

## Known problems

- it is made to be used with GCC
- the concatenation of character sequences has limitations in C preprocessor
- C macro definitions are hard to read and write
- comprehension of the library requires a deeper understanding of how C macros work
- C macro computations can be slow and waste much memory
  - I recommend switching away from C when you have that option
- macro names are fragile, avoid redefining equal names at any cost!
  - It might break loop conditions and cause exponential computations.
- Let's hope, the bug is not in the C preprocessor when the compiler fails because it runs too long or runs out of memory.

## About this project

I was temporarily working on a sensor board at university and out of need or interest, I added this "macro magic" as a hobby within one or two weeks. Not only did it amaze me, that this is possible in C, but it also allowed for a practical experience of theoretical computer science.
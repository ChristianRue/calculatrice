# Calculatrice

A mini console-based computer algebra system in Matlab style syntax. Input is processed line-wise. Type in a valid expression followed by the return key. In what follows we will describe in more detail, what a valid expression is. 

- Spaces are stripped from the input and therefore have no further effect.

- `#` is a valid expression terminating the program.

- Real numbers in usual floating point syntax.

- Mathematical expressions using the operations `+`, `-`, `*`, `/`, `%` (modulo), `^` (power), `!` (factorial) and brackets. Mathematical expressions are evaluated in the proper way respecting the common rules. Moreover `/` can be used as a unary operator for multiplicative inversion just in the same way as `-` can be used as a unary operator for additive inversion, e.g. the expression `/2` is a valid expression for the term `0.5`. Infact both `-` and `/` are internally treated only as unary operators and expressions like `2-1` are translated to `2 + (-1)` before evaluation. The unary operator `!` only accepts positive integers.

- Imaginary numbers can be used via the unary operator `i`, e.g. `1+2i` is a valid expression for the corresponding complex number.

- Matrices in Matlab style syntax, i.e. a matrix is determined by *rows* of same length, separated via `;` and framed by rectangular bracket. Here a *row* is a finite sequence of numbers separated by `,`. So e.g. `[1,2,3;4,5,6]` determines a matrix with 2 rows and 3 columns, the numbers `1,2,3` forming the first and the numbers `4,5,6` the second row. Entries can be complex and even matrices itself, as long as these can be combined to a valid big matrix. So the upper matrix can also be described by `[[1;4],[2,3;5,6]]`, whereas `[1,2;3]` is not a valid expression. 1xn matrices are treated as row vectors, nx1 matrices as columns vectors and 1x1 matrices as complex numbers.

- Shorter expressions for special row vectors following the Matlab syntax, i.e. `[a : b]` determines the row vector with entries `a, a+1, a+2, ...` up to the tallest number `a+n < b` with natural `n`. More generally `[a : b : c]` determines the row vector `a, a+b, a+2b, ...` up to the tallest number `a+bn < c`.

- Mathematical expressions can also be formed with matrices instead of complex numbers, as long as they make sense mathematically. E.g. `/M` computes the inverse of a matrix M, if it is defined. The operators `%` and `!` are only defined for numbers, the power operator `^` accepts only integer values as a second argument, if the first one is a matrix. The operator `'` forms the transpose of a matrix.

- Variables can be defined via `<name> = <expression>`. Valid names for variables are strings consisting of small and capital letters, numbers and underscores. The first character must be a letter. Variables can store matrix values and can be used as these in expressions. Assigned variables are stored in the file `vmem.mem`, so variables are kept after a restart of the program unless you do not delete the file `vmem.mem`. The variable `pi` is pre-defined, but can temporarily be overwritten until the next restart of the program. So the variable `pi` is the only one that will not be stored, but reset after restart. 

- Functions can be defined via `<function_name> = <expression>`. Here `<function_name>` is a valid name (see previous section about variables) followed by a list of names for arguments separated by `,` and bracketed by round brackets, e.g. `f(x,y)`. Of course the expression on the right depends on the arguments on the left, e.g. `f(x,y) = x+y` defines a function called `f` computing the sum of the two arguments x and y. Functions can be evaluated in the usual mathematical way and can be built into mathematical expressions. Like variables also the user-defined functions are stored in a memory file `fmem.mem`. The definition of a function can be recalled by using the expression `<function_name>?`. Like for the variable `pi` there are also built-in functions, that can be overwritten but will be reset after restart. We will only list their names, because a detailed description of their definition can be obtained in the same way as for the user-defined functions:

    `exp, log, sinh, cosh, sin, cos, tan, asin, acos, atan, ceil, floor, abs, sign, min, max, rnd, eye, zero, diag, dim, size, norm, eig, lrl, lrr, qrq, qrr, mrm, mrr, lgs, ker, det, trace, cofactor, sum, prod, wav`

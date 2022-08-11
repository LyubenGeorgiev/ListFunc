<h1 align="center">ListFunc - Functional language interpreter</h1>

### Rules of ListFunc:
```
<real-number> ::= <valid double record>
<list-literal> ::= [<expression0> <expression1> ...]
<function-name> ::= <valid C++ identifier>
<function-call> ::= <function-name>(<expression0>, <expression1> ...)
<expression> ::= <list-literal> | <real-number> | <function-call>
<param-expression> ::= <expression> | #integer | <function-name>([<param-expression>,...])
<function-declaration>::= <function-name> -> <param-expression>
```

### Built-in functions:
```
eq(#0, #1) ::= checks if #0 == #1
le(#0, #1) ::= checks if #0 < #1
nand(#0, #1) ::= NOT AND logical operator
length(#0) ::= returns length of #0, where #0 is finite list
head(#0) ::= return head of #0, where #0 is list
tail(#0) ::= return tail of #0, where #0 is list
list(#0) ::= generates infinite list with starting value #0 and increment step 1
list(#0, #1) ::= generates infinite list with starting value #0 and increment step #1
list(#0, #1, #2) ::= generates finite list with starting value #0, increment step #1 and length of #2
concat(#0, #1) ::= return concatenation of #0 and #1
if(#0, #1, #2) ::= if #0 is true then #1 else #2
read() ::= reads NUMBER from standard input
write(#0) ::= writes #0 on on the standard output and returns 0 when successful otherwise 1
int(#0) ::= converts double to int, acts like trunc()
add(#0, #1) ::= #0 + #1
sub(#0, #1) ::= #0 - #1
mul(#0, #1) ::= #0 * #1
div(#0, #1) ::= #0 / #1
mod(#0, #1) ::= #0 % #1
sqrt(#0) ::= returns sqare root of #0
```

#### Compilation and running for ListFunc:
```
$ make
$ ./ListFunc
$ ./ListFunc <file_path>
```

#### Compilation and running for tests:
```
$ cd test/
$ make
$ ./test
```

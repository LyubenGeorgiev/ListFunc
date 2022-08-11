#include "ListFunc.h"

// pow -> if (eq(#1, 0), 1, if(mod(#1,2), mul(#0, pow(mul(#0, #0), div(#1, 2))), pow(mul(#0, #0), div(#1, 2))))
// fib -> int(div(sub(pow(add(1, sqrt(5)), #0), pow(sub(1, sqrt(5)), #0)), mul(pow(2, #0), sqrt(5))))
// fact -> if(eq(#0, 1), 1, mul(#0, fact(sub(#0,1))))
// fib -> if (le(#0, 2), #0, add(fib(sub(#0,1)), fib(sub(#0,2))))

// not -> nand(#0, 1)
// and -> not(nand(#0, #1))
// divisors -> concat([2], list(3, 2, div(int(sqrt(#0)), 2)))
// containsDevisors -> if(length(#0), if(mod(#1,int(head(#0))), containsDevisors(tail(#0), #1), 1), 0)
// isPrime -> if(eq(#0, 2), 1, not(containsDevisors(divisors(#0), #0)))
// filterPrimes -> if(length(#0), if(isPrime(int(head(#0))), concat([head(#0)], filterPrimes(tail(#0))), filterPrimes(tail(#0))), [])
// primesTo -> filterPrimes(concat([2], list(3, 1, sub(#0, 2))))

// hanoi -> if(eq(#0, 0), 0, head(tail(concat([hanoi(sub(#0,1), #1, #3, #2) write([#0 #1 #2]) hanoi(sub(#0,1), #3, #2, #1)], []))))
// min -> if(#0, if(nand(nand(#1, le(head(#0), head(#1))), 1), min(tail(#0), concat([head(#0)], #1)), min(tail(#0), concat(#1, [head(#0)]))), #1)
// sort -> if(#0, concat([head(min(#0, []))], sort(tail(min(#0, [])))), [])

int main(int argc, const char** argv)
{
    if (argc == 1) // Run the program
    {
        return ListFunc::getInstance().run();
    }
    else if (argc == 2) // Run from file
    {
        return ListFunc::getInstance().run(argv[1]);
    }
    
    return -1;
}
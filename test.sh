#!/bin/sh

cat <<EOF > tmp2.c
#include <stdio.h>
void print_foo() {
    printf("foo\n");
}
int ret21() {
    return 21;
}
int mul2(int n) {
    return n * 2;
}
int add(int n, int m) {
    return n + m;
}
EOF

cc -c tmp2.c
rm tmp2.c

assert() {
    expected="$1"
    input="$2"
    ./succ "$input" > tmp.s
    cc -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert_output() {
    expected="$1"
    input="$2"
    ./succ "$input" > tmp.s
    cc -o tmp tmp.s tmp2.o
    actual=$(./tmp)
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 'main() { return 5+20-4; }'
assert 41 'main() { return  12 + 34 - 5 ; }'
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 4 'main() { return (3+5)/2; }'
assert 42 'main() { return -10*3+72; }'
assert 1 'main() { return 5 == (50 / 10); }'
assert 0 'main() { return 5 == 4; }'
assert 0 'main() { return 5 != (50 / 10); }'
assert 1 'main() { return 5 != 4; }'
assert 1 'main() { return 5 < (6*7); }'
assert 0 'main() { return 5 < (5-2); }'
assert 1 'main() { return 5 <= (6-1); }'
assert 0 'main() { return 5 <= (3-2); }'
assert 0 'main() { return 5 > (6*7); }'
assert 1 'main() { return 5 > (5-2); }'
assert 1 'main() { return 5 >= (6-1); }'
assert 1 'main() { return 5 >= (3-2); }'
assert 42 'main() { a = 42; return a; }'
assert 84 'main() { a = 42; return a * 2; }'
assert 21 'main() { a = 42; b = 2; return a / b; }'
assert 42 'main() { height=12;base=7;area=height*base/2;return area; }'
assert 10 'main() { _=1;__=2;___=3;____=4;return  _+__+___+____; }'
assert 5 'main() { return 5; return 4; }'
assert 7 'main() { a=3*2; b=5; return a/3+b; c=7+13; }'
assert 42 'main() { if (1 == 1) return 42; }'
assert 21 'main() { if (1 == 0) return 42; return 21; }'
assert 42 'main() { if (1 == 1) return 42; else return 11; }'
assert 11 'main() { if (1 == 0) return 42; else return 11; }'
assert 55 'main() { n=10; sum=0; for (i=0; i<=n; i=i+1) sum=sum+i; return sum; }'
assert 10 'main() { i=0; while(i<10) i=i+1; return i; }'
assert 42 'main() { a=0; b=0; if (a == b) { a=40; b=2; } return a+b; }'
assert 55 'main() { n=10; sum=0; for (i=0; i<=n; i=i+1) { sum=sum+i; } return sum; }'
assert_output 'foo' 'main() { print_foo(); }'
assert 42 'main() { return ret21()*2; }'
assert 42 'main() { return mul2(21); }'
assert 42 'main() { a=31; b=11; return add(a, b); }'
assert 42 'main(){return 42;}'
assert 42 'main(){a=42; return 42;}'
assert 5 'main(){return fn();} fn(){a=5;return a;}'
assert 55 'main() { return fib(9); } fib(x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'
assert 32 'main() { return func(1,2); } func(a,b) { c=5; d=6; return c*d+a*b; }'
assert 21 'main() { return addall(1,2,3,4,5,6); } addall(a,b,c,d,e,f) { return a+b+c+d+e+f; }'
assert 3 'main() { x=3; y=&x; return *y; }'
assert 3 'main() { x=3; y=5; z=&y+8; return *z; }'

echo OK
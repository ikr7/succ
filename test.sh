#!/bin/sh

cat <<EOF > tmp2.c
#include <stdio.h>
#include <stdlib.h>
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
void alloc4(int **p, int a, int b, int c, int d) {
    *p = calloc(4, sizeof(int));
    *(*p + 0) = a;
    *(*p + 1) = b;
    *(*p + 2) = c;
    *(*p + 3) = d;
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

assert 0  'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4  'int main() { return (3+5)/2; }'
assert 42 'int main() { return -10*3+72; }'
assert 1  'int main() { return 5 == (50 / 10); }'
assert 0  'int main() { return 5 == 4; }'
assert 0  'int main() { return 5 != (50 / 10); }'
assert 1  'int main() { return 5 != 4; }'
assert 1  'int main() { return 5 < (6*7); }'
assert 0  'int main() { return 5 < (5-2); }'
assert 1  'int main() { return 5 <= (6-1); }'
assert 0  'int main() { return 5 <= (3-2); }'
assert 0  'int main() { return 5 > (6*7); }'
assert 1  'int main() { return 5 > (5-2); }'
assert 1  'int main() { return 5 >= (6-1); }'
assert 1  'int main() { return 5 >= (3-2); }'
assert 42 'int main() { int a; a=42; return a; }'
assert 84 'int main() { int a; a=42; return a * 2; }'
assert 21 'int main() { int a; int b; a=42; b=2; return a / b; }'
assert 42 'int main() { int height; int base; int area; height=12; base=7; area=height*base/2; return area; }'
assert 10 'int main() { int _; int __; int ___; int ____; _=1;__=2;___=3;____=4;return  _+__+___+____; }'
assert 5  'int main() { return 5; return 4; }'
assert 7  'int main() { int a; int b; int c; a=3*2; b=5; return a/3+b; c=7+13; }'
assert 42 'int main() { if (1 == 1) return 42; }'
assert 21 'int main() { if (1 == 0) return 42; return 21; }'
assert 42 'int main() { if (1 == 1) return 42; else return 11; }'
assert 11 'int main() { if (1 == 0) return 42; else return 11; }'
assert 55 'int main() { int n; int sum; int i; n=10; sum=0; for (i=0; i<=n; i=i+1) sum=sum+i; return sum; }'
assert 10 'int main() { int i; i=0; while(i<10) i=i+1; return i; }'
assert 42 'int main() { int a; int b; a=0; b=0; if (a == b) { a=40; b=2; } return a+b; }'
assert 55 'int main() { int n; int sum; int i; n=10; sum=0; for (i=0; i<=n; i=i+1) { sum=sum+i; } return sum; }'
assert_output 'foo' 'int main() { print_foo(); }'
assert 42 'int main() { return ret21()*2; }'
assert 42 'int main() { return mul2(21); }'
assert 42 'int main() { int a; a=31; int b; b=11; return add(a, b); }'
assert 42 'int main() { return 42; }'
assert 42 'int main() { int a; a=42; return 42;}'
assert 5  'int main() { return fn(); } int fn(){ int a; a=5; return a;}'
assert 55 'int main() { return fib(9); } int fib(int x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'
assert 32 'int main() { return func(1,2); } int func(int a, int b) { int c; int d; c=5; d=6; return c*d+a*b; }'
assert 21 'int main() { return addall(1,2,3,4,5,6); } int addall(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; }'
assert 3  'int main() { int x; int y; x=3; y=&x; return *y; }'
assert 10 'int main() { int i; i=0; for(;i<10;){i=i+1;} return i; }'
assert 10 'int main() { int i; i=0; for(;i<10;i=i+1){} return i; }'
assert 42 'int main() { int a; int *b; b = &a; *b = 42; return a; }'
assert 42 'int main() { int a; int *b; int **c; b = &a; c = &b; **c = 42; return a; }'
assert 42 'int main() { int a; int *b; int **c; int ***d; b = &a; c = &b; d = &c; ***d = 42; return a; }'
assert 3 'int main() { int *p; alloc4(&p, 2, 3, 5, 7); int *q; q = p + 1; return *q; }'
assert 2 'int main() { int *p; int *q; alloc4(&p, 2, 3, 5, 7); q = p + 2; return q - p; }'
assert 5 'int main() { int *p; int *q; alloc4(&p, 2, 3, 5, 7); q = p + 3; q = q - 1; return *q; }'
assert 4 'int main() { int x; int *y; return sizeof(x); }'
assert 8 'int main() { int x; int *y; return sizeof(y); }'
assert 4 'int main() { int x; int *y; return sizeof(x + 3); }'
assert 8 'int main() { int x; int *y; return sizeof(y + 3); }'
assert 4 'int main() { int x; int *y; return sizeof(*y); }'
assert 4 'int main() { int x; int *y; return sizeof(1); }'
assert 4 'int main() { int x; int *y; return sizeof(sizeof(1)); }'

echo OK
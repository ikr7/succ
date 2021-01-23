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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 42 '-10*3+72;'
assert 1 '5 == (50 / 10);'
assert 0 '5 == 4;'
assert 0 '5 != (50 / 10);'
assert 1 '5 != 4;'
assert 1 '5 < (6*7);'
assert 0 '5 < (5-2);'
assert 1 '5 <= (6-1);'
assert 0 '5 <= (3-2);'
assert 0 '5 > (6*7);'
assert 1 '5 > (5-2);'
assert 1 '5 >= (6-1);'
assert 1 '5 >= (3-2);'
assert 42 'a = 42; a;'
assert 84 'a = 42; a * 2;'
assert 21 'a = 42; b = 2; a / b;'
assert 42 'height=12;base=7;area=height*base/2;area;'
assert 10 '_=1;__=2;___=3;____=4;_+__+___+____;'
assert 5 'return 5; return 4;'
assert 7 'a=3*2; b=5; return a/3+b; c=7+13;'
assert 42 'if (1 == 1) return 42;'
assert 21 'if (1 == 0) return 42; return 21;'
assert 42 'if (1 == 1) return 42; else return 11;'
assert 11 'if (1 == 0) return 42; else return 11;'
assert 55 'n=10; sum=0; for (i=0; i<=n; i=i+1) sum=sum+i; return sum;'
assert 10 'i=0; while(i<10) i=i+1; return i;'
assert 42 'a=0; b=0; if (a == b) { a=40; b=2; } return a+b;'
assert 55 'n=10; sum=0; for (i=0; i<=n; i=i+1) { sum=sum+i; } return sum;'
assert_output "foo" 'print_foo();'
assert 42 'return ret21()*2;'
assert 42 'return mul2(21);'
assert 42 'a=31; b=11; return add(a, b);'

echo OK
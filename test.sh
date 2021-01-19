#!/bin/sh

assert() {
    expected="$1"
    input="$2"
    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"
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

echo OK
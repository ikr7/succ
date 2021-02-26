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
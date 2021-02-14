succ is (going to be) a tiny C compiler written in C, for learning.

based on [this book](https://www.sigbus.info/compilerbook) by [@rui314](https://github.com/rui314)

## Example of compilable codes:

```c
int main() {
    return fib(9); // returns 55
}
int fib(int x) {
    if (x<=1) return 1;
    return fib(x-1) + fib(x-2);
}
```

```c
int main() {
    int a;
    int *b;
    b = &a;
    *b = 42;
    return a; // returns 42
}
```
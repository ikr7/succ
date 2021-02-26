#!/bin/sh

name="tmp.$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c 32)"

ESC=$(printf '\033')
COLOR_RED="${ESC}[31m"
COLOR_OFF="${ESC}[m"

expected="$1"
input="$2"
./succ "$input" > "$name.s"
cc -o "$name.exe" "$name.s"
./$name.exe
actual="$?"
if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
else
    echo  "${COLOR_RED}$input => $expected expected, but got $actual${COLOR_OFF}"
fi
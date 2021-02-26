CFLAGS=-std=c11 -g -static -Werror -Wall -Wextra -pedantic-errors
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

succ: $(OBJS)
	$(CC) -o succ $(OBJS) $(LDFLAGS)

$(OBJS): succ.h

tmp.expectations.txt: test.txt
	cat test.txt | awk '{print $$1}' > tmp.expectations.txt

tmp.testcases.txt: test.txt
	cat test.txt | awk '{ s = ""; for (i = 2; i <= NF; i++) s = s $$i " "; print s }' > tmp.testcases.txt

test: succ tmp.expectations.txt tmp.testcases.txt
	./prep-test.sh
	parallel ./assert.sh :::: tmp.expectations.txt ::::+ tmp.testcases.txt
	rm tmp.*.s tmp.*.exe

clean:
	rm -f succ *.o *~ tmp*

.PHONY: test clean
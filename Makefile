CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

succ: $(OBJS)
	$(CC) -o succ $(OBJS) $(LDFLAGS)

$(OBJS): succ.h

test: succ
	./test.sh

clean:
	rm -f succ *.o *~ tmp*

.PHONY: test clean
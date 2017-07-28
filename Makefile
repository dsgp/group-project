SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)

CFLAGS := -Wall -Werror=implicit-function-declaration -Wextra -g -O0 -std=c99

all: spacewire

spacewire: $(OBJS)
	$(CC) $^ -o $@

%.o: %.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) *.o spacewire

.PHONY: clean

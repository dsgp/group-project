SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)

CFLAGS := -Wall -Wextra -g -O0

all: spacewire

spacewire: $(OBJS)
	$(CC) $^ -o $@

%.o: %.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) *.o spacewire

.PHONY: clean

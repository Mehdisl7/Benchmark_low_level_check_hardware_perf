CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11 -pthread
LDFLAGS =
TARGET = sysprobe

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean

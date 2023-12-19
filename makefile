CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lrt

SRCS = polynomCalc1.c polynomCalc2a.c polynomCalc2b.c polynomCalc3a.c polynomCalc3b.c
OBJS = $(SRCS:.c=.o)
EXECS = $(OBJS:.o=)

all: $(EXECS)

$(EXECS): $(OBJS)
	$(CC) $(CFLAGS) $@.o -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECS)


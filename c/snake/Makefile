
CC=gcc
PROGRAM=snake
SRC= main.c snake.c
OBJS= main.o snake.o
CFLAGS= -c
LDFLAGS=-lncurses

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
$(OBJS): $(SRC)
	$(CC) $(CFLAGS) $(SRC)

clean:
	rm -f $(OBJS) $(PROGRAM)



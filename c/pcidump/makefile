
CC=gcc
PROGRAM=pcidump
CFLAGS= -c
SRC= main.c pci.c
OBJS= main.o pci.o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)

$(OBJS): $(SRC)
	$(CC) $(CFLAGS) $(SRC)

clean:
	rm -f $(OBJS) $(PROGRAM)

#
#
#
PROGRAM=argchk
CC=@cc
CFLAGS=-Wall -c
OBJS= argtest.o
ECHO=@echo

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)

$(OBJS): argtest.c
	$(CC) $(CFLAGS) argtest.c

	$(ECHO) build ok!
clean:
	@rm -f $(PROGRAM) $(OBJS)

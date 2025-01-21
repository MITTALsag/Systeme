CC=gcc
CFLAGS=-g 
CPPFLAGS=-Wall -Werror

PROGS=copy.run

INCLUDE =
OBJS =

.PHONY: all
all: $(PROGS)

%.run: %.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<


.PHONY: clean
clean:
	$(RM) $(PROGS) *.o *~

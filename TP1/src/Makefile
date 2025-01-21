CC=gcc
CFLAGS=-g 
CPPFLAGS=-Wall -Werror

PROGS=simple_prog.run testz.run affiche_PID_PPID.run arbre_fork.run exec_cmd.run

INCLUDE = utils.h
OBJS = utils.o

.PHONY: all
all: $(PROGS)

%.run: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

%.o: %.c $(INCLUDE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<


.PHONY: clean
clean:
	$(RM) $(PROGS) *.o *~

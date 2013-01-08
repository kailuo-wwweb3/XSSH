## The name of the C compiler.  The default is gcc
CC = gcc

## -g Tells compiler to include extra debugging information in its output
## -Wall Gives warnings of constructs that look suspect
## -ansi Tells the compiler to enforce ANSI C standards 
## -pedantic More pedantic ansi, warnings for stuff you probably didn't mean. 
## See http://www.shlomifish.org/lecture/W2L/Development/slides/gcc/flags.html
## for more gcc flags.
CFLAGS = -ansi -pedantic -g -Wall 

## A list of options to pass to the linker. 
LDFLAGS = $(CFLAGS) 

## Name the executable program, list header files and source files
PROG = xssh
HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)

## Give the object files the same names as the source files.
## Only change the extension from .c to .o.
OBJS = $(SRCS:.c=.o)

## Build the program from the object files
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS) -lc -lreadline -lcurses

## Rules for the source files.  They compile each X.c to make X.o
%.o: %.c %.h $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $< -lc -lreadline -lcurses

## Remove all the compilation and debugging files
clean: 
	rm -f core $(PROG) $(OBJS) *~

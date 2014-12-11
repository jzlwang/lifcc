# Top-level Makefile for Project 1.
# 
# Targets:
#   gmake [ default ]        Compiles sources in src directory
#   gmake check              Compiles sources and runs tests.  The parameter
#                            APYC indicates which compiler to run.
#   gmake clean              Removes unnecessary files, including those that
#                            are reproducible.

# The compiler to use while compiling
CC = gcc

# Flags to use while compiling.
CFLAGS = -std=gnu99 -Wall -g

# List of normal C sources.  Add any new ones here
SRCS = compiler.c lexer.c parser.c code_gen.c util/util.c util/smap.c util/amap.c

.PHONY: default compile check clean

default: compile

compile: ../lifcc

../lifcc: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

check: 
	$(MAKE) -C .. check

clean:
	$(RM) *~ *.o util/*~ util/*.o 

# Top-level Makefile for Project 1.
# 
# Targets:
#   gmake [ default ]        Compiles sources in src directory
#   gmake check              Compiles sources and runs tests.  The parameter
#                            APYC indicates which compiler to run.
#   gmake clean              Removes unnecessary files, including those that
#                            are reproducible.


.PHONY: default compile check clean

default: compile

compile:
	$(MAKE) -C src compile

check:
	$(MAKE) -C tests check

clean:
	$(RM) *~ lifcc
	$(MAKE) -C src clean
	$(MAKE) -C tests clean

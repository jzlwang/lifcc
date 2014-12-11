.PHONY: check clean really-clean

LIFCC = ../lifcc

GOODTEST = python test_proj1.py 

BADTEST = python test_proj1.py --errors

check: 
	$(MAKE) -C .. compile
	@echo Testing correct programs...
	rm -f errflag
	-$(GOODTEST) --compiler=$(LIFCC) --keep correct/*.lc || touch errflag
	@echo
	@echo Testing erroneous programs...
	-$(BADTEST) --compiler=$(LIFCC) error/*.lc || touch errflag
	@if test -f errflag; then code=1; else code=0; fi; \
        rm -f errflag; exit $$code

clean:
	$(RM) errflag *~ */*~ *.s */*.s

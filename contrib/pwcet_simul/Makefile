CFLAGS=-O0 -Wall -g
LDFLAGS=-O0 -g
LIBS=-lrt

.PHONY: generated clean test mrproper all

all: simul test

simul: simul.o libpwcet.a 

pwcet-private.o: pwcet-private.c pwcet-private.h pwcet-public.h taskdata.h

simul.o: simul.c pwcet-public.h taskdata.h

libpwcet.a: pwcet-private.o cache.o taskdata.o
	ar r libpwcet.a pwcet-private.o cache.o taskdata.o
	ranlib libpwcet.a

taskdata.o: taskdata.c pwcet-private.h pwcet-public.h taskdata.h

cache.o: cache.c pwcet-private.h pwcet-public.h taskdata.h cache.h

douter.o: douter.c pwcet-private.h

douter: douter.o libpwcet.a 

test: douter
	./douter

clean:
	rm -f simul *.o *~ *.a douter DEADJOE core*

mrproper: clean
	rm -f taskdata.h taskdata.c

generated: taskdata.c taskdata.h

taskdata.c: $(wildcard /home/clement/klemmCloud/travail/code/otawa-demo/taskdata.c)
	cp /home/clement/klemmCloud/travail/code/otawa-demo/taskdata.c taskdata.c

taskdata.h: $(wildcard /home/clement/klemmCloud/travail/code/otawa-demo/taskdata.h)
	cp /home/clement/klemmCloud/travail/code/otawa-demo/taskdata.h taskdata.h

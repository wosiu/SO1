all: manager executor

manager: manager.c err.o commons.o executor
	cc -g -ggdb -Wall -o manager manager.c err.o commons.o

executor: executor.c err.o commons.o
	cc -g -ggdb -Wall -o executor executor.c err.o commons.o

err.o: err.c err.h
	cc -Wall -c err.c -o err.o

commons.o: commons.c commons.h
	cc -g -ggdb -Wall -c commons.c -o commons.o

clean:
	rm -f *.o manager executor


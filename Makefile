all: manager executor

manager: manager.c err.o executor
	cc -Wall -o manager manager.c err.o

executor: executor.c err.o
	cc -Wall -o executor executor.c err.o

err.o: err.c err.h
	cc -Wall -c err.c -o err.o

clean:
	rm -f *.o manager executor


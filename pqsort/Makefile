all: generate run

generate: generate.c
	gcc -o generate generate.c

pqueue: pqueue.c
	gcc -c -g -lm pqueue.c

aligned_malloc_free.o: aligned_malloc_free.c
	gcc -c -g -lm aligned_malloc_free.c

linearBarrier.o: linearBarrier.c 
	gcc -c -g -lm linearBarrier.c

#logbarrier.o: logbarrier.c 
#	gcc -c -g -lm logbarrier.c

pqsort.o: pqsort.c linearBarrier.o  aligned_malloc_free.o pqueue
	gcc -c -g -lm -lpthread pqsort.c

run: driver.c pqsort.o 
	gcc -o  run driver.c pqsort.o linearBarrier.o  aligned_malloc_free.o pqueue.o -lm -lpthread 

clean:
	rm generate pqsort.o run   linearBarrier.o pqueue.o aligned_malloc_free.o

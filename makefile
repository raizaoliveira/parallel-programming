all:
	gcc -c rastriginfunction.c -Wall -Wextra
	gcc -c model.c -Wall
	gcc -c mtrand.c
	ar rcs libmodel.a *.o
	mpicc -o ppd main.c -L. -lmodel -lm

clean:
	rm ppd *~

#ifndef _MERSENNE_H_
#define _MERSENNE_H_

#include "rastriginfunction.h"

double mmm(char **population);

double make_hamming(char *ind1, char *ind2);

void drop_pop(char *X);

int convert(char x);

int create_models1(char **pop, double modelo[][SIZE], int F, double average);


#endif

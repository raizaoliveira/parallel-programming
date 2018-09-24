#ifndef _RASTRIGINFUNCTION_H_
#define _RASTRIGINFUNCTION_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define PI 3.14
#define POPULATION 500/*tamanho da populacao*/
#define SIZE 10		/*tamanho do individuo*/
#define MUT_PROB 0.3
#define MUT_SH 0.04
#define alpha 0.05



double evaluate_individual(char* individual);

#endif


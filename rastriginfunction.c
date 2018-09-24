#include "rastriginfunction.h"

/*Calcula o ftness-> valor real para cada individuo
Dominio da função esta no intervalo[-5,12 ; 5,12]
*/
double evaluate_individual(char* individual)
{
	int i, number;
	double fitness;
	double x[SIZE] = {0};
	int n;
	n = SIZE;
	number = 0;
	for(i = 0; i < SIZE; i++)/*transfoma cada individuo em uma numero inteiro*/
		if (individual[i] == '1')
		{
			n--;
			number += pow(2, n);
			
		}

	number++;
	for(i = 0; i < SIZE; i++)
	{	
		x[i] = number;
		x[i] *= 0.00015625;
		x[i] -= 5.12;
	}
        fitness = 0;
        for(i = 0; i < SIZE; i++)
		fitness += pow(x[i], 2) - 10*cos(2*PI*x[i]);
  
	return fitness;
}




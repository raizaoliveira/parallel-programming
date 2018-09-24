#include "model.h"

/*media da metada dos melhores individuos de cada populacao*/
double mmm(char **population)
{
	register int i, j;
	double best[POPULATION];
	double t, sum;

	for(i = 0; i < POPULATION; i++)
		best[i] = evaluate_individual(population[i]);
	
	
	for (i = 1 ; i <= POPULATION - 1; i++)/*ordena a populacao*/
	{
		j = i;
 		while ( j > 0 && best[j] < best[j-1])
		{
			t = best[j];
			best[j]   = best[j-1];
			best[j-1] = t;
 			j--;
		}
	}
	sum = 0;
	for(i = 0; i < POPULATION / 2; i++)/*soma a melhor metade*/
	{
		sum += best[i];
	}

	sum = sum / (POPULATION/2);
	

	return sum;

}

double make_hamming(char *ind1, char *ind2)
{
	/*
	se P2 < F entao o individuo2 pra gerar outro modelo, senao avalia-se o proximo individuo
	o Fator F sera 25%, 50%, 75% e 100% para os escravos*/
	
	double P2; /*representa a proximidade dos individuos*/
	register int i;
	int p = 0;

	for (i = 0; i < SIZE; i++)
		if (ind1[i] == ind2[i])
			p++;

		
	P2 = (p * 100)/(SIZE);
	return P2;

}

void drop_pop(char *X)
{

	register int i;
	double P[SIZE];

	for (i = 0; i < SIZE; i++)
		P[i+1] = P[i] * (1.0 - alpha) + X[i] * alpha;

}


/*recebe caracter binario e retorna um int */
int convert(char x)
{
	if (x == '1')
		return 1;

	else
		return 0;
	

}
/*criacao dos modelos probabilisticos
tem como parametros a matiz de populacao
matriz do modelo
F -> porcentagem de similaridade (25, 50, 75 ou 100)
average -> media da metade dos melhores de cada populacao
retorna a quantidade de modelos criados para cada populacao
*/
int create_models1(char **pop, double modelo[][SIZE], int F, double average)
{
	double P2;
	int i, j;
	int nmodel;
	nmodel = 0;
	for(i = 0; i < POPULATION; i++)
	{
		if( i == 0)
		{
			nmodel++;
			for(j = 0; j < SIZE; j++)
			{
				modelo[nmodel][j] = ((0.5 - alpha) * convert(pop[i][j]) + alpha * average) / 100;
			}
		}
		if( i > 0)
		{
			P2 = make_hamming(pop[i - 1], pop[i]);/*distancia de hamming no individuo  corrente com o anterior*/
			if(P2 < F)
			{
				nmodel++;
				for(j = 0; j < SIZE; j++)
				{
					modelo[nmodel][j] = ((0.5 - alpha) * convert(pop[i][j]) + alpha * average) / 100;
				}
			}

		}
	}
	return nmodel;
	
}



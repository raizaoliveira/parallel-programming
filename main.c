#include <stdio.h>
#include <stdlib.h>
#include "link.h"
#include "mtrand.h"
#include "model.h"
#include "mpi.h"



int size, rank, source, tag;
int qtd1, qtd2, qtd3, qtd4;


char** initialize_population()
{
	int i,j,random;
	char** population = malloc(POPULATION*sizeof(char*));
	mt_srand (99999999999);
	for(i = 0; i < POPULATION; i++)
		population[i] = malloc(SIZE*sizeof(char));
    
	for(i = 0; i < POPULATION; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			random = mt_rand()%2;
			population[i][j] = (char)(random+48);
		}
	}
	return population;
}





/*Escravo*/

int ord(char population[][SIZE])
{
	register int i, j;
	double best[POPULATION];
	int addr[POPULATION];
	double t, taddr;

	for(i = 0; i < POPULATION; i++)
	{
		best[i] = evaluate_individual(population[i]);
		addr[i] = i;
	}
	
	
	for (i = 1 ; i <= POPULATION - 1; i++)
	{
		j = i;
 		while ( j > 0 && best[j] < best[j-1])
		{
			t = best[j];
			taddr = addr[j];
			best[j] = best[j-1];
			addr[j] = addr[j-1];
			best[j-1] = t;
			addr[j-1] = taddr;
 			j--;
		}
	}
	return addr[0];
}


char individual(double gene)
{
	double gen;
	mt_srand (99999999999);

	gen = mt_rand_1 ();
	if (gen < gene)
		return '1';
	else 
		return '0';


}

/*comparar se o individuo criado ja existe*/
int remake_ind(int line, char *individuo, char pop[][SIZE])
{
	double individuo_x;
	double current_individuo;
	int i, j;
	char  *ind_pop;
	ind_pop = malloc (SIZE * sizeof(char));
	current_individuo = evaluate_individual(individuo);
	for (i = 0; i < line; i++)
	{
		for(j = 0; j < SIZE; j++)
			ind_pop[j] = pop[i][j];
	
		individuo_x = evaluate_individual(ind_pop);

		if (current_individuo == individuo_x)
			return 1;	
	}

	return 0;

}


/*
tamanho do modelo varia de acordo com F
O numero de colunas é fixo em SIZE
Numero de linhas variavel de acordo com F
escravo recebe vetor de modelo probabilistco e gera matriz de pop*/
void initialize_species_populations(char population[][POPULATION][SIZE],double model[][SIZE], int nmodel)
{
	char msg;
	int i,j,k,random,species, n;
	char *individuo;
	int equal;
	species = 20;
	individuo = malloc (POPULATION * sizeof(char));
	for(i = 0; i < nmodel; i++)
	{
		for(j = 0; j < POPULATION; j++)
		{
			for(k = 0; k < SIZE; k++)
			{
				msg = individual(model[j][i]);
				population[i][k][j] = msg;
			}
				for(n = 0; n < SIZE; n++)
				{
					individuo[n] = population[i][j][n];
				}
				if(remake_ind( j, individuo, population[i]))
				{	
					mt_srand (99999999999);
					equal = mt_rand ()%POPULATION;
					random = mt_rand()%2;
					population[i][j][equal] = (char)(random+48);
				}
		}
	}
}

int main(int argc, char **argv)
{
				
	MPI_Status stat;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	tag = 0;
	if (rank == 0)
	{
		
		int LEN_MODEL;
		int i, j, k;
		int N1, N2, N3, N4;
		double modelo1[POPULATION][SIZE], modelo2[POPULATION][SIZE], modelo3[POPULATION][SIZE], modelo4[POPULATION][SIZE];
		char **POP_1, **POP_2, **POP_3, **POP_4;
		double best1, best2, best3, best4;

		POP_1 = initialize_population();/*Inicializa as populacoes aleatoriamente*/
		POP_2 = initialize_population();
		POP_3 = initialize_population();
		POP_4 = initialize_population();

		best1 = mmm(POP_1);/*Calcula a media da metada dos melhores individuos de cada populacao*/
		best2 = mmm(POP_2);
		best3 = mmm(POP_3);
		best4 = mmm(POP_4);
	

		/*Cria os modelos probabilisticos*/
		N1 = create_models1(POP_1, modelo1, 25, best1);
		printf("N1 %d\n", N1);
		N2 = create_models1(POP_2, modelo2, 50, best2);
		printf("N2 %d\n", N2);
		N3 = create_models1(POP_3, modelo3, 75, best3);
		printf("N3 %d\n", N3);
		N4 = create_models1(POP_4, modelo4, 100, best4);
		printf("N4 %d\n", N4);
	
		/*Evia para cada escravo a quantidade de modelos que foi gerado para ele*/
		MPI_Send(&N1, 1, MPI_INT, 1, 70, MPI_COMM_WORLD);
		MPI_Send(&N2, 1, MPI_INT, 2, 70, MPI_COMM_WORLD);
		MPI_Send(&N3, 1, MPI_INT, 3, 70, MPI_COMM_WORLD);
		MPI_Send(&N4, 1, MPI_INT, 4, 70, MPI_COMM_WORLD);

	
		/*A cada iteracao envia um modelo para o seu respectivo escravo*/
		for(i = 0; i < N1; i++)
			MPI_Send(modelo1[i], SIZE, MPI_DOUBLE, 1, 70, MPI_COMM_WORLD);
		
		for(i = 0; i < N2; i++)
			MPI_Send(modelo2[i], SIZE, MPI_DOUBLE, 2, 70, MPI_COMM_WORLD);
		
		for(i = 0; i < N3; i++)
			MPI_Send(modelo3[i], SIZE, MPI_DOUBLE, 3, 70, MPI_COMM_WORLD);

		for(i = 0; i < N4; i++)
			MPI_Send(modelo4[i], SIZE, MPI_DOUBLE, 4, 70, MPI_COMM_WORLD);
	}
	else{
		if(rank == 1)
		{
			int qtd;
			int i, j, k, counter;
			int best_addr;
			double bst;
			char best[POPULATION][SIZE];
			counter = 0;
			mt_srand (99999999999);
			MPI_Recv(&qtd, 1 , MPI_INT, 0, 70,  MPI_COMM_WORLD, &stat);/*recebe a quantidade de modelos que sera enviado*/
			double modelo[qtd][SIZE];
			
			for(i = 0; i < qtd; i++)/*recebe os modelos e os armazena em uma matriz com a quntidade recebida de modelos*/
				MPI_Recv(modelo[i], SIZE, MPI_DOUBLE, 0, 70, MPI_COMM_WORLD, &stat);

		
			char populations[qtd][POPULATION][SIZE];/*inicializa as populacaos a partir dos modelos recebidos*/
			initialize_species_populations(populations,modelo, qtd);

			best_addr = ord(populations[i]);/*O melhor individuo de cada populacao*/
			
			do{
					initialize_species_populations(populations,modelo, qtd);

					best_addr = ord(populations[i]);

					for(i = 0; i < qtd; i++)
						for(j = 1; j <= SIZE; j++)
						{	/*atualiza cada modelo a partir do melhor individuo das populacoes*/
							modelo[i][j-1] = modelo[i][j] * (1.0 - alpha) + best[best_addr][j-1] *alpha;
							if (modelo[i][j-1] < MUT_SH)
							{
								modelo[i][j-1] = modelo[i][j-1] *(1.0 - MUT_SH) + (mt_rand()%2) * MUT_SH; 

							}
						}

					bst = evaluate_individual(populations[i][j]);
					printf("Escravo 1-> %d: %f\n", counter, bst);
					counter++;
				
			}while(counter < 1000);

			
		}
		if(rank == 2)
		{
			int qtd;
			int i, j, k, counter;
			int best_addr;
			double bst;
			char best[POPULATION][SIZE];
			counter = 0;
			mt_srand (99999999999);
			MPI_Recv(&qtd, 1 , MPI_INT, 0, 70,  MPI_COMM_WORLD, &stat);
			double modelo[qtd][SIZE];
			
			for(i = 0; i < qtd; i++)
				MPI_Recv(modelo[i], SIZE, MPI_DOUBLE, 0, 70, MPI_COMM_WORLD, &stat);

			char populations[qtd][POPULATION][SIZE];
			initialize_species_populations(populations,modelo, qtd);


			do{
					initialize_species_populations(populations,modelo, qtd);

					best_addr = ord(populations[i]);

					for(i = 0; i < qtd; i++)
						for(j = 1; j <= SIZE; j++)
						{
							modelo[i][j-1] = modelo[i][j] * (1.0 - alpha) + best[best_addr][j-1] *alpha;
							if (modelo[i][j-1] < MUT_SH)
							{
								modelo[i][j-1] = modelo[i][j-1] *(1.0 - MUT_SH) + (mt_rand()%2) * MUT_SH; 

							}
						}

					bst = evaluate_individual(populations[i][j]);
					printf("Escravo 2 -> %d: %f\n", counter, bst);
					counter++;
				
			}while(counter < 1000);
		}
		if(rank == 3)
		{
			int qtd;
			int i, j, k, counter;
			int best_addr;
			double bst;
			char best[POPULATION][SIZE];
			counter = 0;
			mt_srand (99999999999);
			MPI_Recv(&qtd, 1 , MPI_INT, 0, 70,  MPI_COMM_WORLD, &stat);
			double modelo[qtd][SIZE];
			
			for(i = 0; i < qtd; i++)
				MPI_Recv(modelo[i], POPULATION, MPI_DOUBLE, 0, 70, MPI_COMM_WORLD, &stat);

			char populations[qtd][POPULATION][SIZE];
			initialize_species_populations(populations,modelo, qtd);


			do{
					initialize_species_populations(populations,modelo, qtd);

					best_addr = ord(populations[i]);

					for(i = 0; i < qtd; i++)
						for(j = 1; j <= SIZE; j++)
						{
							modelo[i][j-1] = modelo[i][j] * (1.0 - alpha) + best[best_addr][j-1] *alpha;
							if (modelo[i][j-1] < MUT_SH)
							{
								modelo[i][j-1] = modelo[i][j-1] *(1.0 - MUT_SH) + (mt_rand()%2) * MUT_SH; 

							}
						}

					bst = evaluate_individual(populations[i][j]);
					printf("Escravo 3 ->%d: %f\n", counter, bst);
					counter++;
				
			}while(counter < 1000);	
		}
		if(rank == 4)
		{
			int qtd;
			int i, j, k, counter;
			int best_addr;
			double bst;
			char best[POPULATION][SIZE];
			mt_srand (99999999999);
			counter = 0;
			MPI_Recv(&qtd, 1 , MPI_INT, 0, 70,  MPI_COMM_WORLD, &stat);
			double modelo[qtd][SIZE];
			
			for(i = 0; i < qtd; i++)
				MPI_Recv(modelo[i], SIZE, MPI_DOUBLE, 0, 70, MPI_COMM_WORLD, &stat);

			
			char populations[qtd][POPULATION][SIZE];


			do{
					initialize_species_populations(populations,modelo, qtd);

					best_addr = ord(populations[i]);

					for(i = 0; i < qtd; i++)
						for(j = 1; j <= SIZE; j++)
						{
							modelo[i][j-1] = modelo[i][j] * (1.0 - alpha) + best[best_addr][j-1] *alpha;
							if (modelo[i][j-1] < MUT_SH)
							{
								modelo[i][j-1] = modelo[i][j-1] *(1.0 - MUT_SH) + (mt_rand()%2) * MUT_SH; 

							}
						}

					bst = evaluate_individual(populations[i][j]);
					printf("Escravo 4 -> %d: %f\n", counter, bst);
					counter++;
				
			}while(counter < 1000);


		}
	}

	MPI_Finalize();

	return 0;
}



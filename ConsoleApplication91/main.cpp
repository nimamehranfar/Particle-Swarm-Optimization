#include "systemc.h"
#include <cstring>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <chrono> 

using namespace std::chrono;
using namespace std;

#define rand_1 ((float)rand() / (float)RAND_MAX)

const int numofdims = 30;
const int numofparticles = 3;
float Xmin[30], Xmax[30];
float initialpop[3][30];
float worsts[256], bests[256];
float gbest[30];
float fitnesses[numofparticles];
float avragefitness[256];


void fitnessfunc(float X[numofparticles][numofdims], float fitnesses[numofparticles])
{
	memset(fitnesses, 0, sizeof(float) * numofparticles);
	for (int i = 0; i < numofparticles; i++)
	{
		for (int j = 0; j < numofdims; j++)
		{
			fitnesses[i] += (pow(X[i][j], 2));
		}
	}
}

float mean(float inputval[], int vallength)
{
	int addvalue = 0;
	for (int i = 0; i < vallength; i++)
	{
		addvalue += inputval[i];
	}
	return (float)(addvalue / vallength);
}

void PSO(int numofiterations, float c1, float c2, float *gbestfit, float gbest[numofdims])
{
	float V[numofparticles][numofdims] = { 0 };
	float X[numofparticles][numofdims];
	float Vmax[numofdims];
	float Vmin[numofdims];
	float pbests[numofparticles][numofdims];
	float pbestfits[numofparticles];
	float fitnesses[numofparticles];
	float w;
	float minfit;
	int   minfitidx;

	memcpy(X, initialpop, sizeof(float) * numofparticles * numofdims);
	fitnessfunc(X, fitnesses);
	minfit = *min_element(fitnesses, fitnesses + numofparticles);
	minfitidx = min_element(fitnesses, fitnesses + numofparticles) - fitnesses;
	*gbestfit = minfit;
	memcpy(gbest, X[minfitidx], sizeof(float) * numofdims);

	for (int i = 0; i < numofdims; i++)
	{
		Vmax[i] = 0.2 * (Xmax[i] - Xmin[i]);
		Vmin[i] = -Vmax[i];
	}

	for (int t = 0; t < numofiterations; t++)
	{
		w = rand_1;

		for (int i = 0; i < numofparticles; i++)
		{
			if (fitnesses[i] < pbestfits[i])
			{
				pbestfits[i] = fitnesses[i];
				memcpy(pbests[i], X[i], sizeof(float) * numofdims);
			}
		}
		for (int i = 0; i < numofparticles; i++)
		{
			for (int j = 0; j < numofdims; j++)
			{
				V[i][j] = min(max((w * V[i][j] + rand_1 * c1 * (pbests[i][j] - X[i][j])
					+ rand_1 * c2 * (gbest[j] - X[i][j])), Vmin[j]), Vmax[j]);
				X[i][j] = min(max((X[i][j] + V[i][j]), Xmin[j]), Xmax[j]);
			}
		}

		fitnessfunc(X, fitnesses);
		minfit = *min_element(fitnesses, fitnesses + numofparticles);
		minfitidx = min_element(fitnesses, fitnesses + numofparticles) - fitnesses;
		if (minfit < *gbestfit)
		{
			*gbestfit = minfit;
			memcpy(gbest, X[minfitidx], sizeof(float) * numofdims);
		}

		worsts[t] = *max_element(fitnesses, fitnesses + numofparticles);
		bests[t] = *gbestfit;
		avragefitness[t] = mean(fitnesses, numofparticles);
	}


}

int sc_main(int argc, char* argv[]) {
	time_t t;
	auto start = high_resolution_clock::now();
	srand((unsigned)time(&t));
	float gbestfit;
	float c1 = 2;
	float c2 = 2;
	
	for (int i = 0; i < 30; i++)
	{
		Xmax[i] = 30;
		Xmax[i] = -30;
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 30; j++)
		{
			initialpop[i][j] = rand() % (61) - 30;
		}

	PSO(1000, c1, c2, &gbestfit, gbest);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "fitness: " << gbestfit << endl << "avrage fitness: " << *avragefitness << endl;
	cout << "time (ms): " << duration.count() << endl;
	
	return 0;
}
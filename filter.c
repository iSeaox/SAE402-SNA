/*
	File: filter.c
	Author: Guillaume Seimandi
	Description:
		Implement function that create FIR digital filter
		Implement function to use a FIR on incomming data
        Implement native window functions
*/
//-------------------------------------------------------------------------------------------------
// --------------- INCLUDES ---------------
// --- Private Includes ---
#include "Include/filter.h"
#include "Include/main.h"

//-------------------------------------------------------------------------------------------------
// --------------- Fonctions ---------------
int Window_BlackmanHarris(double* pOut, unsigned int num)
{
  const double a0      = 0.35875f;
  const double a1      = 0.48829f;
  const double a2      = 0.14128f;
  const double a3      = 0.01168f;
  unsigned int idx    = 0;
 
	while(idx < num)
	{
		pOut[idx] = a0 - (a1 * cosf( (2.0f * M_PI * idx) / (num - 1) )) + 
				 (a2 * cosf( (4.0f * M_PI * idx) / (num - 1) )) - 
				 (a3 * cosf( (6.0f * M_PI * idx) / (num - 1) ));
		idx++;
	}
	return(1);
}
//-------------------------------------------------------------------------------------------------
int Window_Hamming(double* pOut, unsigned int num)
{
  const double a0 = 0.54f;
  const double a1 = 0.46f;
  unsigned int idx = 0;
 
	while(idx < num)
	{
		pOut[idx] = a0 - (a1 * cosf( (2.0f * M_PI * idx) / (num - 1) ));
		idx++;
	}
	return(1);
}
//-------------------------------------------------------------------------------------------------
int Window_Flattop(double* pOut, unsigned int num)
{
  const double a0 = 0.21557895f;
  const double a1 = 0.41663158f;
  const double a2 = 0.277263158f;
  const double a3 = 0.083578947f;
  const double a4 = 0.006947368f;
  unsigned int idx = 0;
 
	while(idx < num)
	{
		pOut[idx] = a0 - (a1 * cosf( (2.0f * M_PI * idx) / (num - 1) )) + 
				 (a2 * cosf( (4.0f * M_PI * idx) / (num - 1) )) - 
				 (a3 * cosf( (6.0f * M_PI * idx) / (num - 1) )) +
				 (a4 * cosf( (8.0f * M_PI * idx) / (num - 1) ));
		idx++;
	}
	return(1);
}
//-------------------------------------------------------------------------------------------------
void CreateLowPass(double* hp, double freqC, int (*appodWindow)(double*, unsigned int)) {
    int i;
	double KV = (2.0 * freqC * TE);
    double appodCoeff[NB_FILTER_COEF];

    // Calcul coefficients h[i] du filtre FIR LOWPASS
    for(i=-(NB_FILTER_COEF/2) ; i <(NB_FILTER_COEF/2); i++)
	{
		if(i == 0)
		{
			hp[i+(NB_FILTER_COEF/2)] = KV * 1.0;
		}
		else
		{
			hp[i+(NB_FILTER_COEF/2)] = KV * (sinf(2.0 * PI * freqC * TE * i) / (2.0 * PI * freqC * TE * i));
		}
	}

    // Calcul fenetre d'appodisation
	appodWindow(&appodCoeff[0], NB_FILTER_COEF);
	
    // Appodisation de la RI
	for(i = 0 ; i < NB_FILTER_COEF ; i++) {
		hp[i] *= appodCoeff[i];
	}
}
//-------------------------------------------------------------------------------------------------
void CreateHighPass(double* hp, double freqC, int (*appodWindow)(double*, unsigned int)) {
	int i;
	double KV = (2.0 * freqC * TE);
    double appodCoeff[NB_FILTER_COEF];

    // Calcul coefficients h[i] du filtre FIR HIGHPASS
	for(i=-(NB_FILTER_COEF/2) ; i <(NB_FILTER_COEF/2); i++)
	{
		if(i == 0)
		{
			hp[i+(NB_FILTER_COEF/2)] = 1.0 - KV;
		}
		else
		{
			hp[i+(NB_FILTER_COEF/2)] = -1.0 * KV * (sinf(2.0 * PI * freqC * TE * i) / (2.0 * PI * freqC * TE * i));
		}
	}

    // Calcul fenetre d'appodisation
	appodWindow(&appodCoeff[0], NB_FILTER_COEF);
	
    // Appodisation de la RI
	for(i = 0 ; i < NB_FILTER_COEF ; i++) {
		hp[i] *= appodCoeff[i];
	}
}
//-------------------------------------------------------------------------------------------------
void CreateBandPass(double* hp, double freqCLow, double freqCHigh, int (*appodWindow)(double*, unsigned int)) {

	int i;
	double Klow = (2.0 * freqCLow * TE);
	double Khigh = (2.0 * freqCHigh * TE);

	double h1;
	double h2;

    double appodCoeff[NB_FILTER_COEF];

 	for(i=-(NB_FILTER_COEF/2) ; i <(NB_FILTER_COEF/2); i++)
	{
		if(i == 0)
		{
			hp[i+(NB_FILTER_COEF/2)] = Khigh - Klow;
		}
		else
		{
			h1 = Klow * (sinf(2.0 * PI * freqCLow * TE * i) / (2.0 * PI * freqCLow * TE * i));
			h2 = Khigh * (sinf(2.0 * PI * freqCHigh * TE * i) / (2.0 * PI * freqCHigh * TE * i));
			hp[i+(NB_FILTER_COEF/2)] = h2 - h1;
		}
	}

	// Calcul fenetre d'appodisation
	appodWindow(&appodCoeff[0], NB_FILTER_COEF);
	
    // Appodisation de la RI
	for(i = 0 ; i < NB_FILTER_COEF ; i++) {
		hp[i] *= appodCoeff[i];
	}
}
//-------------------------------------------------------------------------------------------------
double ProcessFIR(double signalEn, double* ptrTableEn, double* ptrHp)
{
	// static int deadtimer = 4;
	// deadtimer--;
	// int i;
	// double Sn = 0;
	// printf("------------------------------------\n");
	// for(i = NB_FILTER_COEF - 1 ; i >= NB_FILTER_COEF / 2 ; i--)
	// {
		
	// 	// printf("------------\n");
	// 	// -- Viellisement de l'entrée --
	// 	// ptrTableEn[i] = ptrTableEn[i - 1];
	// 	*(ptrTableEn + i) = *(ptrTableEn + i - 1); // Viellisement à la volée par la droite
	// 	// printf("%d -> %d\n", i, i - 1);

	// 	if(i == NB_FILTER_COEF - 1) { 
	// 		(*(ptrTableEn)) = signalEn; // on fait rentrer signalEn
	// 		// printf("0 -> En\n");
	// 		// printf("test: %lf", signalEn);
	// 	}
	// 	else{
	// 		*(ptrTableEn + NB_FILTER_COEF - 1 - i) = *(ptrTableEn + NB_FILTER_COEF - 2 - i);// Viellisement à la volée par la gauche
	// 		// printf("%d -> %d\n", NB_FILTER_COEF - 1 - i, NB_FILTER_COEF - 2 - i);
	// 	}

	// 	// -- Calcul du produit de convolution --
		
	// 	Sn +=  ((*(ptrTableEn + i)) +  (*(ptrTableEn + NB_FILTER_COEF - 1 - i))) * (*(ptrHp + i));
	// 	// printf("(x%d + x%d) * h%d\n", i, NB_FILTER_COEF - 1 - i, i);
	// 	// printf("h%d = %lf\n", i, (*(ptrHp + i)));
	// 	// printf("Sn: %lf\n", Sn);

	// 	// printf("-----------------\n[");
	// 	// for(i=0 ; i < NB_FILTER_COEF ; i++)
	// 	// {
	// 	// 	printf("%lf", ptrTableEn[i]);
	// 	// 	if(i + 1 != NB_FILTER_COEF) printf(", ");
	// 	// }
	// 	// printf("]\n-------------------\n");
		
		
	// }
	// printf("En: %lf\n", signalEn);
	// printf("[");
	// for(i=0 ; i < NB_FILTER_COEF ; i++)
	// {
	// 	printf("%lf", ptrTableEn[i]);
	// 	if(i + 1 != NB_FILTER_COEF) printf(", ");
	// }
	// printf("]\n");
	// printf("------------------------------------\n");
	// if(deadtimer == 0) exit(1);

	// // printf("------------------------------------");
	
	// return Sn;
	int i;
	double Sn = 0;

	// ici, xn = signalEn -> ptrTableEn est un buffer de type FIFO
	// on commence à itérer en NBCOEF - 2 car le dernier échantillon du tableau (donc le plus vieux) n'est pas prit en compte
	// => qu'on itère dans le sens de index décroissant
	// en gros on a en equivalent : tableEn -> |En|x2|x1|x0| et hp -> |h0|h1|h2|h3|
	//		En * h0              0 * 0
	//		x2 * h1   en indice  1 * 1
	// 		x1 * h2      ->      2 * 2
	// 		x0 * h3              3 * 3

	for(i = NB_FILTER_COEF - 1 ; i > 0 ; i--) {
		ptrTableEn[i] = ptrTableEn[i - 1]; // veillissement
	}
	ptrTableEn[0] = signalEn; // on fait rentrer signalEn

	for(i = 0 ; i < NB_FILTER_COEF / 2 ; i++) {
		Sn += (ptrTableEn[i] + ptrTableEn[NB_FILTER_COEF - 1 - i]) * ptrHp[i];
	}
	return Sn;
}
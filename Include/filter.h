/*
	File: filter.h
	Author: Guillaume Seimandi
	Description:
		Define of function that create FIR digital filter
		Define prototype of function to use a FIR on incomming data
        Define prototype of native windows fonction
*/
//-------------------------------------------------------------------------------------------------
#if !defined(FILTER_DEFINE)
#define FILTER_DEFINE

//-------------------------------------------------------------------------------------------------
// --------------- INCLUDES ---------------
// --- Public Includes ---
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <unistd.h> // FOR POSIX

// --------------- DEFINES ---------------
#define NB_FILTER_COEF 64

//-------------------------------------------------------------------------------------------------
// --------------- Prototypes ---------------
int Window_BlackmanHarris(double* pOut, unsigned int num);
int Window_Hamming(double* pOut, unsigned int num);
int Window_Flattop(double* pOut, unsigned int num);

void CreateLowPass(double* hp, double freqC, int (*appodWindow)(double*, unsigned int));
void CreateHighPass(double* hp, double freqC, int (*appodWindow)(double*, unsigned int));
void CreateBandPass(double* hp, double freqCLow, double freqCHigh, int (*appodWindow)(double*, unsigned int));
double ProcessFIR(double signalEn, double* ptrTableEn, double* ptrHp);

#endif // FILTER_DEFINE

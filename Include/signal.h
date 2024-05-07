/*
	File: signal.h
	Author: Guillaume Seimandi
	Description:
		Define and implement function to create Harmonic family or white noise base signal
*/
//-------------------------------------------------------------------------------------------------

#if !defined(SIGNAL_DEFINE)
#define SIGNAL_DEFINE

// --------------- INCLUDES ---------------
// --- Public Includes ---
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// --- Private Includes ---
#include "main.h"
#include "noise.h"

// --------------- DEFINES ---------------
// --- Macros ---
#define DEG_TO_RAD(angle) angle * (PI / 180.0);

//-------------------------------------------------------------------------------------------------
// --------------- Structs ---------------
typedef struct Sine
{
    double Ampli;
    double Freq;
    double Phase;
} Sine;

typedef struct SineFamily
{
    Sine* SineTab;
    double KV;
    uint8_t Size;
} SineFamily;

//-------------------------------------------------------------------------------------------------
//
double getNextSampleSF(SineFamily* sf_ptr, int k) {
    double ret = 0;
    Sine* sinePtr;

    for(int i = 0 ; i < sf_ptr->Size ; i++) {
        sinePtr = ((sf_ptr->SineTab) + i);
        ret += sinePtr->Ampli * sin(2 * PI * sinePtr->Freq * TE * k + sinePtr->Phase);
    }
    return ret * sf_ptr->KV;
}

double getNextSampleNoise() {
    return (double)noise() * (16384.0/2.0);
}

#endif // SIGNAL_DEFINE

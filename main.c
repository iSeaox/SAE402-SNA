/*
	File: main.c
	Author: Guillaume Seimandi
	Description:
		Entry point of program.
		Setup of base signal, FIR digital filter, AM and FM modulation
*/
//-------------------------------------------------------------------------------------------------
// --------------- INCLUDES ---------------
// --- Public Includes ---
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <unistd.h> // FOR POSIX System

// --- Private Includes ---
#include "Include/main.h"
#include "Include/files.h"
#include "Include/signal.h"
#include "Include/filter.h"
#include "Include/modulation.h"

//-------------------------------------------------------------------------------------------------
// --------------- MAIN ---------------
int main(int argc, char const *argv[])
{
    PCM16_stereo_t  *buffer_p = NULL;
    FILE* fileWav_ptr = NULL;
    FILE* fileCad_ptr = NULL;

    char wavFileName[80] = {"./out/outWav.wav"};
    char dataFileName[80] = {"./out/outCAD.prn"};

    double duration;
    uint32_t FrameCount;
    uint32_t written;
    int ret;

	double filterCoef[NB_FILTER_COEF];
	double tableEn[NB_FILTER_COEF] = {0};

    double Signal;
    int k;
    int i;

	// ----- Base Signal -----
    Sine sineTab[4];
	// | Ampli | Freq | Phase |
    Sine sine1 = {1, 300, 0};
    sineTab[0] = sine1;

    Sine sine2 = {1, 400, 0};
    sineTab[1] = sine2;

    Sine sine3 = {1, 500, 0};
    sineTab[2] = sine3;

    Sine sine4 = {1, 600, 0};
    sineTab[3] = sine4;

	// | SineTab | KV | Size |
    SineFamily sFamily = {sineTab, 32767.0 / 4.0, 4};

	// ----- Filters -----
	CreateLowPass(filterCoef, 600.0, &Window_BlackmanHarris);
	// CreateBandPass(filterCoef, 750.0, 850.0, &Window_BlackmanHarris);
	// if(!write_prn_file("LPF_SE_Coeff.prn", filterCoef, NB_FILTER_COEF)) {
	// 	printf("File LPF_Coeff.prn successfully wrote | NbCoeff : %d\n", NB_FILTER_COEF);
	// }
	// ----- Modulation -----
	// | KFM | IndiceMod | FArr | FDep | F0 | X | Offset | IncremAngleLaw |
	// ModFM modulationFMConst = {32767.0 / 1.2, (PI / 32768) / 50.0f, 1400, 0, 0, 0.0, 0.0, &ModFMConstantLaw};
	ModFM modulationFMLinear = {32767.0 / 1.2, (PI / (32768.0)), 100, 110, 1, 0.0, 0.0, &ModFMSawoothLaw};
	// ModFM modulationFMSine = {32767.0 / 1.2, (PI / 32768) / 50.0f, 2200, 1200, 4.0, 0.0, 0.0, &ModFMSinLaw};
    // -----------
	ModAM_Harmonic harmonics[MODAM_NB_HARMONIC];
	ModAM_Harmonic h0 = {1, -0.061, PI / 2.0};
	harmonics[0] = h0;
	ModAM_Harmonic h1 = {2, -0.048, 0};
	harmonics[1] = h1;
	ModAM_Harmonic h2 = {3, -0.039, 0};
	harmonics[2] = h2;
	ModAM_Harmonic h3 = {4, -0.247, 0};
	harmonics[3] = h3;
	ModAM_Harmonic h4 = {5, 0.017, 0};
	harmonics[4] = h4;
	ModAM_Harmonic h5 = {6, -0.016, 0};
	harmonics[5] = h5;
	ModAM_Harmonic h6 = {8, -0.123, 0};
	harmonics[6] = h6;
	ModAM_Harmonic h7 = {10, -0.01, 0};
	harmonics[7] = h7;

	ModAM modulationAM = {harmonics, 0.388, 0.5, 0.01, 1.0, 0.0};
	// ModAM modulationAM = {harmonics, 0.388, 0.5, 0.1, 1.0, 0.0};
	//-------------------------------------------

    printf("----- Combo -----\n");
  	printf("Start signal generation with FE = %lf\n", FE);
 	duration = 10.0; // seconds
	FrameCount = (duration * SAMPLE_RATE);

	// Allocate WAV data buffer
	buffer_p = allocate_PCM16_stereo_buffer(FrameCount);
	if(NULL == buffer_p)
	{
		perror("fopen failed in main");
		exit(-1);
	}
 
 	// Create CAD File
	fileCad_ptr = fopen(dataFileName, "w");
	if(NULL == fileCad_ptr)
	{
		perror("fopen failed in main");
		exit(-1);
	}

    //-----------------------------------------------------------------------------------------
	// SIGNAL GENERATION
	k = 0;
	// int16_t maxSignal = 0;
	// int16_t minSignal = 32767;
  	for(i=0 ; i<FrameCount ; i++)
  	{
		if(k * sFamily.SineTab[0].Freq * TE > 1)
			k = 0;

		// Signal = getNextSampleSF(&sFamily, k);
		Signal = getNextSampleNoise();
		Signal = ProcessFIR(Signal, tableEn, filterCoef);
		Signal = ProcessModulationAM((ModAM*) &modulationAM, Signal);
		Signal = ProcessModulationFM((ModFM*) &modulationFMLinear, Signal);
		
		
		k++;
		//---------------------------------------
		// Store datas in Wav buffer
		buffer_p[i].left    = (int16_t)(Signal);
		buffer_p[i].right   = (int16_t)(Signal);
		//---------------------------------------
		// Write datas in PRN file, for Mathcad Prime
		fprintf(fileCad_ptr, "%+01.6f\n", Signal);
	}
	fclose(fileCad_ptr);

	// Affichage des niveaux max et min pour le réglage de KV
	// printf("Max Signal : %d | Min Signal : %d\n", maxSignal, minSignal);

  	printf("\nData File generated .... Framecount=%d\n", FrameCount);
	
	//-----------------------------------------------------------------------------------------
	// WAVE FILE GENERATION
	
	// Create the Wav file
	fileWav_ptr = fopen(wavFileName, "wb");
	if(NULL == fileWav_ptr)
	{
		perror("fopen failed in main");
		exit(-1);
	}

 	// Write the Wav file header
	ret = write_PCM16_stereo_header(fileWav_ptr, SAMPLE_RATE, FrameCount);
	if(ret < 0)
	{
		perror("write_PCM16_stereo_header failed in main");
		fclose(fileWav_ptr);
		exit(-1);
	}

	// Write the Wav data out to file
	written = write_PCM16wav_data(fileWav_ptr, FrameCount, buffer_p);
	if(written < FrameCount)
	{
		perror("write_PCM16wav_data failed in main");
		fclose(fileWav_ptr);
		exit(-1);
	}

    fclose(fileWav_ptr);
	//-----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------

    free(buffer_p);
        
  	printf("\nWave File generated .... Framecount=%d\n\n", FrameCount);
 	printf("\n\n");

    return 0;
}

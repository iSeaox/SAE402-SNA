/*
	File: modulation.h
	Author: Guillaume Seimandi
	Description:
		Define and implement function to handle AM and FM modulation
		Define struct to handle AM and FM modulation
*/
//-------------------------------------------------------------------------------------------------
#if !defined(MODULATION_DEFINE)
#define MODULATION_DEFINE

//-------------------------------------------------------------------------------------------------
// --------------- INCLUDES ---------------
// --- Public Includes ---
#include <math.h>

// --- Private Includes ---
#include "main.h"

//-------------------------------------------------------------------------------------------------
// --------------- DEFINES ---------------
#define MODAM_NB_HARMONIC 8

//-----------------------------------------------------------------------------------------
// --------- FM Modulation ---------
// --- Structs ---
typedef struct ModFM 
{
    double Kfm;
    double IndiceMod;
    double FArr;
    double FDep;
    double F0;
    double X;
    double Offset;
    double (*IncremAngleLaw)(void*);
} ModFM;
//-----------------------------------------------------------------------------------------
// --- Functions ---
double ModFMConstantLaw(void* mod) {
    return 2 * PI * TE * (((ModFM*)mod)->FArr);
}
//-----------------------------------------------------------------------------------------
double ModFMHeliceLaw(void* mod) {
    double excursion = (((ModFM*)mod)->FArr) - (((ModFM*)mod)->FDep);
    return PI * TE * ((excursion * (((ModFM*)mod)->X)) + (((ModFM*)mod)->FDep));
}
//-----------------------------------------------------------------------------------------
double ModFMSawoothLaw(void* mod) {
    double excursion = (((ModFM*)mod)->FArr) - (((ModFM*)mod)->FDep);
    return 2 * PI * TE * ((excursion * (((ModFM*)mod)->X)) + (((ModFM*)mod)->FDep));
}
//-----------------------------------------------------------------------------------------
double ModFMSinLaw(void* mod) {
    double excursion = (((ModFM*)mod)->FArr) - (((ModFM*)mod)->FDep);
    return 2 * PI * TE * (((sin(2 * PI * (((ModFM*)mod)->X)) + 1.0) * (0.5 * excursion)) + (((ModFM*)mod)->FDep));
}
//-----------------------------------------------------------------------------------------
double ProcessModulationFM(ModFM* mod, double signal) {
    double signalFM;

    signalFM = mod->Kfm * sin(mod->Offset + (mod->IndiceMod * signal));

    mod->Offset += mod->IncremAngleLaw(mod);
    mod->X += 2 * PI * TE * mod->F0;

    if(mod->Offset > 2 * PI) {
        mod->Offset -= 2 * PI;
    }

    if(mod->X > 1) {
        mod->X = 0;
    }
    return signalFM;
}

//-----------------------------------------------------------------------------------------
// --------- AM Modulation ---------
// --- Structs ---
typedef struct ModAM_Harmonic 
{
    double Rang;
    double Niv;
    double Phase;
} ModAM_Harmonic;
//-----------------------------------------------------------------------------------------
typedef struct ModAM
{
    ModAM_Harmonic* Harmonics;
    double NivCont;
    double IndiceAM;
    double SeuilBas;
    double F0;
    double X;
} ModAM;
//-----------------------------------------------------------------------------------------
// --- Functions ---
double ProcessModulationAM(ModAM* mod, double signal) {
    double signalAM;
    double signalAMMod = mod->NivCont;
    int i;

    for(i = 0 ; i < MODAM_NB_HARMONIC ; i++) {
        signalAMMod += mod->Harmonics[i].Niv * sin(2 * PI * mod->Harmonics[i].Rang * mod->X) + mod->Harmonics[i].Phase;
    }
    signalAMMod *= mod->IndiceAM;

    if(signalAMMod < mod->SeuilBas) {
        signalAMMod = 0.0;
    }

    signalAM = signalAMMod * signal;
    mod->X += TE * mod->F0;

    if(mod->X >= 1) {
        mod->X = 0;
    }

    return signalAM;
}
#endif // MODULATION_DEFINE

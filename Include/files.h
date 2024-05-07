/*
	File: files.h
	Author: Guillaume Seimandi
	Description:
		Define prototype of function that handle Wav file and generate prn based on an array
        Define structs and constant to handle Wav file
*/
//-------------------------------------------------------------------------------------------------
#if !defined(FILES_DEFINE)
#define FILES_DEFINE

//-------------------------------------------------------------------------------------------------
// --------------- INCLUDES ---------------
// --- Public Includes ---
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------
// --------------- DEFINES ---------------
#define SUBCHUNK1SIZE   16
#define AUDIO_FORMAT    1 // For PCM
#define NUM_CHANNELS    2
#define SAMPLE_RATE     10000
#define BITS_PER_SAMPLE 16
#define BYTE_RATE       (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8)
#define BLOCK_ALIGN     (NUM_CHANNELS * BITS_PER_SAMPLE / 8)

//-------------------------------------------------------------------------------------------------
// --------------- Structs ---------------
// Structures definition section
// Data structure to hold a single frame with two channels
typedef struct PCM16_stereo_s
{
    int16_t left;
    int16_t right;
} PCM16_stereo_t;

// The header of a wav file Based on: https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
typedef struct wavfile_header_s
{
    char    ChunkID[4];     //   4   
    int32_t ChunkSize;      //   4   
    char    Format[4];      //   4   
    char    Subchunk1ID[4]; //   4   
    int32_t Subchunk1Size;  //   4   
    int16_t AudioFormat;    //   2   
    int16_t NumChannels;    //   2   
    int32_t SampleRate;     //   4   
    int32_t ByteRate;       //   4   
    int16_t BlockAlign;     //   2   
    int16_t BitsPerSample;  //   2   
    
    char    Subchunk2ID[4];
    int32_t Subchunk2Size;
} wavfile_header_t;

//-------------------------------------------------------------------------------------------------
// --------------- Prototypes ---------------
// Write Wav header file function
// Return 0 on success and -1 on failure
int write_PCM16_stereo_header(FILE *file_p, uint32_t SampleRate, uint32_t FrameCount);

// Allocate buffer for Wav datas
// Return pointer on buffer allocated, NULL pointer if allaction fail
PCM16_stereo_t *allocate_PCM16_stereo_buffer(uint32_t FrameCount);

// Write Wav file datas
// Return the number of audio frames sucessfully written
int32_t write_PCM16wav_data(FILE* file_p, uint32_t FrameCount, PCM16_stereo_t *buffer_p);

// Read Wav file datas
// Return the number of audio frames sucessfully read
int32_t  read_PCM16wav_data(FILE* file_p, uint32_t FrameCount, PCM16_stereo_t *buffer_p);

// Save array as PRN
// Return 0 on success and -1 on failure
int write_prn_file(char const *fileName, double* data, int size);


#endif // MAIN_DEFINE

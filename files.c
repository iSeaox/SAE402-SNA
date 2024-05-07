/*
	File: files.c
	Author: Guillaume Seimandi
	Description:
		Implementation of files.h
		Implement function to handle Wav file and generate prn based on an array
*/
//-------------------------------------------------------------------------------------------------
// --------------- INCLUDES ---------------
// --- Public Includes ---
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// --- Private Includes ---
#include "Include/files.h"

//-------------------------------------------------------------------------------------------------
// --------------- Fonctions ---------------
// Write Wav header file function
// Return 0 on success and -1 on failure
int write_PCM16_stereo_header(FILE *file_p, uint32_t SampleRate, uint32_t FrameCount)
{
  int ret;
  wavfile_header_t wav_header;
  uint32_t subchunk2_size;
  uint32_t chunk_size;
  size_t write_count;

	subchunk2_size  = FrameCount * NUM_CHANNELS * BITS_PER_SAMPLE / 8;
	chunk_size      = 4 + (8 + SUBCHUNK1SIZE) + (8 + subchunk2_size);

	wav_header.ChunkID[0] = 'R';
	wav_header.ChunkID[1] = 'I';
	wav_header.ChunkID[2] = 'F';
	wav_header.ChunkID[3] = 'F';

	wav_header.ChunkSize = chunk_size;

	wav_header.Format[0] = 'W';
	wav_header.Format[1] = 'A';
	wav_header.Format[2] = 'V';
	wav_header.Format[3] = 'E';

	wav_header.Subchunk1ID[0] = 'f';
	wav_header.Subchunk1ID[1] = 'm';
	wav_header.Subchunk1ID[2] = 't';
	wav_header.Subchunk1ID[3] = ' ';

	wav_header.Subchunk1Size = SUBCHUNK1SIZE;
	wav_header.AudioFormat = AUDIO_FORMAT;
	wav_header.NumChannels = NUM_CHANNELS;
	wav_header.SampleRate = SampleRate;
	wav_header.ByteRate = BYTE_RATE;
	wav_header.BlockAlign = BLOCK_ALIGN;
	wav_header.BitsPerSample = BITS_PER_SAMPLE;

	wav_header.Subchunk2ID[0] = 'd';
	wav_header.Subchunk2ID[1] = 'a';
	wav_header.Subchunk2ID[2] = 't';
	wav_header.Subchunk2ID[3] = 'a';
	wav_header.Subchunk2Size = subchunk2_size;

	write_count = fwrite(&wav_header, sizeof(wavfile_header_t), 1, file_p);
		    
	ret = (1 != write_count)? -1 : 0;
	return ret;
}

//-------------------------------------------------------------------------------------------------
// Allocate buffer for Wav datas
// Return pointer on buffer allocated, NULL pointer if allaction fail
PCM16_stereo_t *allocate_PCM16_stereo_buffer(uint32_t FrameCount)
{
	return (PCM16_stereo_t *)calloc(FrameCount, sizeof(PCM16_stereo_t));
}

//-------------------------------------------------------------------------------------------------
// Write Wav file datas
// Return the number of audio frames sucessfully written
int32_t write_PCM16wav_data(FILE* file_p, uint32_t FrameCount, PCM16_stereo_t *buffer_p)
{
  int32_t ret;
    
	ret = fwrite(buffer_p, sizeof(PCM16_stereo_t), FrameCount, file_p);
	return ret;
}

//-------------------------------------------------------------------------------------------------
// Read Wav file datas
// Return the number of audio frames sucessfully read
int32_t  read_PCM16wav_data(FILE* file_p, uint32_t FrameCount, PCM16_stereo_t *buffer_p)
{
  int32_t ret;
    	
	ret = fread(buffer_p, sizeof(PCM16_stereo_t), FrameCount, file_p);
	return ret;
}

//-------------------------------------------------------------------------------------------------
// Save array as PRN
// Return 0 on success and -1 on failure
int write_prn_file(char const *fileName, double* data, int size) {
	int i;
	FILE* file_p = fopen(fileName, "w");

	if(file_p == NULL) {
		char errorMsg[128];
		sprintf("Can't open %s\n", fileName);
		perror(errorMsg);
		return -1;
	}

	for(i = 0 ; i < size ; i++) {
		fprintf(file_p, "%+01.6f\n", data[i]);
	}
	fclose(file_p);
	return 0;
}

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <cstdint>
#include <string>
#include <vector>

/**
* A struct that can hold the data of a wav-file
*/
struct WAVHeader {
	char riffHeader[4];    // "RIFF"
	uint32_t wavSize;      // Size of the WAV file
	char waveHeader[4];    // "WAVE"
	char fmtHeader[4];     // "fmt "
	uint32_t fmtChunkSize; // Size of the fmt chunk
	uint16_t audioFormat;  // Audio format (1 = PCM)
	uint16_t numChannels;  // Number of channels
	uint32_t sampleRate;   // Sample rate
	uint32_t byteRate;     // Byte rate
	uint16_t blockAlign;   // Block align
	uint16_t bitsPerSample;// Bits per sample
	char dataHeader[4];    // "data"
	uint32_t dataSize;     // Size of the data section
	std::vector<int16_t> data;    // The actual data
};

/**
* A function that outputs the data of the WAVHeader
* @param[in]	wav	The audiofile written into the WAVHeader.
*/
void output_wav_data(WAVHeader& wav);

/**
* A function that outputs the data of the WAVHeader
* @param[in]	file_path	The path to the audiofile.
* @return	wav		The audio file written into the WAVHeader struct.
*/
WAVHeader read_wav_file(std::string file_path);

/**
* A function that writes the audiodata to a new wav file.
* @param[in]	header	The audiofile written into the WAVHeader struct.
* @param[in]	filename	The name of the new file.
*/
void write_wav_file(WAVHeader& header, std::string filename);

#endif

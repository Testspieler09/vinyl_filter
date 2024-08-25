#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <cstdint>

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
};

// Function declarations (prototypes)
WAVHeader read_wav_file(const char* file_path);
void write_wav_file(WAVHeader& header);

#endif

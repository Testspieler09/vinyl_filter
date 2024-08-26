#include "filehandler.h"
#include <iostream>
#include <cstring>
#include <fstream>

WAVHeader read_wav_file(const char* file) {
	WAVHeader wav;

	// All checks for correct wav file
	std::ifstream wave_file(file, std::ios::binary);
	if (!wave_file) {
		throw "Failed to open input file.\n";
	}

	wave_file.read(wav.riffHeader, 4);
	if (std::strncmp(wav.riffHeader, "RIFF", 4) != 0) {
		throw "File seams to be currupted!\n";
	}

	wave_file.read(reinterpret_cast<char*>(&wav.wavSize), sizeof(wav.wavSize));

	wave_file.read(wav.waveHeader, 4);
	if (std::strncmp(wav.waveHeader, "WAVE", 4) != 0) {
		throw "Not a valid WAVE file.\n";
	}

	wave_file.read(wav.fmtHeader, 4);
	if (std::strncmp(wav.fmtHeader, "fmt ", 4) != 0) {
		throw "Missing 'fmt ' subchunk.\n";
	}

	wave_file.read(reinterpret_cast<char*>(&wav.fmtChunkSize), sizeof(wav.fmtChunkSize));
	if (wav.fmtChunkSize != 16) { // For PCM, the chunk size should be 16
		throw "Unexpected fmt chunk size.\n";
	}

	wave_file.read(reinterpret_cast<char*>(&wav.audioFormat), sizeof(wav.audioFormat));
	if (wav.audioFormat != 1) {
		throw "Unsupported audio format (only PCM is supported).\n";
	}

	wave_file.read(reinterpret_cast<char*>(&wav.numChannels), sizeof(wav.numChannels));
	wave_file.read(reinterpret_cast<char*>(&wav.sampleRate), sizeof(wav.sampleRate));
	wave_file.read(reinterpret_cast<char*>(&wav.byteRate), sizeof(wav.byteRate));
	wave_file.read(reinterpret_cast<char*>(&wav.blockAlign), sizeof(wav.blockAlign));
	wave_file.read(reinterpret_cast<char*>(&wav.bitsPerSample), sizeof(wav.bitsPerSample));
	if (wav.byteRate != wav.sampleRate * wav.numChannels * wav.bitsPerSample / 8) {
		throw "Byte rate seams to be wrong\n";
	} else if (wav.blockAlign != wav.numChannels * wav.bitsPerSample / 8) {
		throw "Block align seams to be wrong\n";
	}

	wave_file.read(wav.dataHeader, 4);
	if (std::strncmp(wav.dataHeader, "data", 4) != 0) {
		throw "Missing 'data' subchunk.\n";
	}

	wave_file.read(reinterpret_cast<char*>(&wav.dataSize), sizeof(wav.dataSize));
	if (wav.wavSize != 36 + wav.dataSize) {
		throw "File seams to be currupted\n";
	}

	// Output the read data
	std::cout << "RIFF Header: " << std::string(wav.riffHeader, 4) << std::endl;
	std::cout << "File Size: " << wav.wavSize << std::endl;
	std::cout << "WAVE Header: " << std::string(wav.waveHeader, 4) << std::endl;
	std::cout << "FMT Header: " << std::string(wav.fmtHeader, 4) << std::endl;
	std::cout << "FMT Chunk Size: " << wav.fmtChunkSize << std::endl;
	std::cout << "Audio Format: " << wav.audioFormat << std::endl;
	std::cout << "Number of Channels: " << wav.numChannels << std::endl;
	std::cout << "Sample Rate: " << wav.sampleRate << std::endl;
	std::cout << "Byte Rate: " << wav.byteRate << std::endl;
	std::cout << "Block Align: " << wav.blockAlign << std::endl;
	std::cout << "Bits Per Sample: " << wav.bitsPerSample << std::endl;
	std::cout << "Data Header: " << std::string(wav.dataHeader, 4) << std::endl;
	std::cout << "Data Size: " << wav.dataSize << std::endl;
	wav.data.resize(wav.dataSize);
	wave_file.read(reinterpret_cast<char*>(wav.data.data()), wav.dataSize);
	if (!wave_file) {
		throw "Error reading the WAV file data.";
	}

	wave_file.close();

	return wav;
}

void write_wav_file(WAVHeader& wav) {
	return;
}

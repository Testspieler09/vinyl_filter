#include <iostream>
#include "filters.h"
#include "filehandler.h"

int main(int argc, char* argv[]) {

	if (argc > 1) {
		const char* file = argv[1];
		try {
			WAVHeader file_data = read_wav_file(file);
			output_wav_data(file_data);
			write_wav_file(file_data, "vinyl.wav");
		} catch (const char* error) {
			std::cerr << "Error: " << error << std::endl;
		}
	} else {
		std::cout
			<< "Please provide the path to the file you want to sound like vinyl."
			<< std::endl;
	}

	return 0;
}

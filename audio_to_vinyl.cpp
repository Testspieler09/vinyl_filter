#include <iostream>
#include "filters.h"
#include "filehandler.h"

int main(int argc, char* argv[]) {

	if (argc > 1) {
		const char* file = argv[1];
		try {
			read_wav_file(file);
		} catch (const char* error) {
			std::cerr << "Error: " << error << std::endl;
		}
	}

	return 0;
}

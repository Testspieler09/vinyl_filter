#include <iostream>
#include <argparse/argparse.hpp>
#include "filters.hpp"
#include "filehandler.hpp"

int main(int argc, char* argv[]) {

	argparse::ArgumentParser program("Audio to Vinyl",
				  "AudioToVinyl Version 0.0.0");

	// later also folderpath possible
	program.add_argument("Sourcepath")
		.help("The path to the file you want to convert.")
		.required();
	// other arguments

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	auto file = program.get<std::string>("Sourcepath");
	std::cout << file << std::endl;
	try {
		WAVHeader file_data = read_wav_file(file);
		output_wav_data(file_data);
		write_wav_file(file_data, "vinyl.wav");
	} catch (const char* error) {
		std::cerr << "Error: " << error << std::endl;
	}

	return 0;
}

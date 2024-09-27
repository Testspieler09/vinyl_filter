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
	program.add_argument("Outputpath")
		.help("The path to the output folder")
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
	auto output_path = program.get<std::string>("Outputpath");
	try {
		WAVHeader file_data = read_wav_file(file);
		double track_length = calc_audio_length(file_data);
		output_wav_data(file_data);
		limit_bit_depth(file_data, 16);
		limit_sampling_rate(file_data, 10000);
		limit_dynamic_range(file_data, {-10000, 10000});
		shorten_audio(file_data, track_length);
		output_wav_data(file_data);
		std::string output = generate_file_name(output_path, base_name(file));
		write_wav_file(file_data, output);
	} catch (const char* error) {
		std::cerr << "Error: " << error << std::endl;
	}

	return 0;
}

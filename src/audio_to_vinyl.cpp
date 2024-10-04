#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <argparse/argparse.hpp>
#include "filters.hpp"
#include "filehandler.hpp"

struct Settings {
	uint32_t sampleRate = 48000;
	std::vector<double> dynamicRange = {55.0, 65.0};
	uint16_t bitDepth = 24;
	int cracklingNoise_lvl = 0;
	int general_noise_lvl = 0;
	// needle settings missing rn
};

void run_procedure(std::string file, std::string output_path, const Settings& settings) {
	WAVHeader file_data;
	try {
		file_data = read_wav_file(file);
	} catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::exit(1);
	}

	std::string output;
	try {
		output = generate_file_name(output_path, base_name(file));
	} catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::exit(1);
	}

	double track_length = calc_audio_length(file_data); // for the shortening later

	// Apply filters
	add_crackle_noise(file_data, settings.cracklingNoise_lvl);
	add_pop_click_noise(file_data, settings.general_noise_lvl);
	limit_bit_depth(file_data, settings.bitDepth);
	limit_sampling_rate(file_data, settings.sampleRate);
	limit_dynamic_range(file_data, settings.dynamicRange);
	shorten_audio(file_data, track_length);

	// Write the data to a file
	write_wav_file(file_data, output);
	return;
}

int main(int argc, char* argv[]) {

	argparse::ArgumentParser program("Audio to Vinyl",
				  "AudioToVinyl Version 0.0.0");

	program.add_argument("Sourcepath")
		.help("The path to the file(s) you want to convert.")
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
		Settings settings;
		// iterate over argv and set the argv stuff as settings if given
		if (std::filesystem::is_directory(file)) {
			for (const auto& entry : std::filesystem::directory_iterator(file)) {
				if (entry.is_regular_file() && entry.path().extension() == ".wav") {
					run_procedure(entry.path().string(), output_path, settings);
				}
			}
		} else {
			run_procedure(file, output_path, settings);
		}
	} catch (const char* error) {
		std::cerr << "Error: " << error << std::endl;
	}

	return 0;
}

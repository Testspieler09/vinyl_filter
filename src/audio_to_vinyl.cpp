#include "filehandler.hpp"
#include "filters.hpp"
#include <argparse/argparse.hpp>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>

void run_procedure(std::string file, std::string output_path,
                   const Settings &settings) {
  WAVHeader file_data;
  try {
    file_data = read_wav_file(file);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::exit(1);
  }

  std::string output;
  try {
    output = generate_file_name(output_path, base_name(file));
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::exit(1);
  }

  double track_length =
      calc_audio_length(file_data); // for the shortening later

  // Apply filters
  add_crackle_noise(file_data, settings.crackling_noise_lvl);
  add_pop_click_noise(file_data, settings.general_noise_lvl);
  limit_bit_depth(file_data, settings.bit_depth);
  adjust_sampling_rate(file_data, settings.sample_rate);
  resize_audio(file_data, track_length);

  /*
   * important to apply the needle sounds after limiting the original audio as
   * the realworld sounds should not be limited
   */
  add_start_needle(file_data, settings.needle_drop_duration);
  add_end_needle(file_data, settings.needle_lift_duration);

  // Write the data to a file
  write_wav_file(file_data, output);
  return;
}

int main(int argc, char *argv[]) {

  Settings settings;

  argparse::ArgumentParser program("Audio to Vinyl",
                                   "AudioToVinyl Version 0.0.0");

  // Required arguments
  program.add_argument("Sourcepath")
      .help("The path to the file(s) you want to convert.")
      .required();
  program.add_argument("Outputpath")
      .help("The path to the output folder")
      .required();

  // Optional arguments
  program.add_argument("-s", "--samples")
      .help("The number of samples you want in 1Hz")
      .nargs(1)
      .default_value(settings.sample_rate)
      .scan<'i', uint32_t>();
  program.add_argument("-bD", "--bitDepth")
      .help("The bit depth you want in 1 Bit")
      .nargs(1)
      .default_value(settings.bit_depth)
      .scan<'i', uint16_t>();
  program.add_argument("-cNL", "--cracklingNoiseLvl")
      .help("The amount of crackling noise you want in 0.01%")
      .nargs(1)
      .default_value(settings.crackling_noise_lvl)
      .scan<'i', int>();
  program.add_argument("-gNL", "--generalNoiseLvl")
      .help("The amount of white noise you want in 0.001%")
      .nargs(1)
      .default_value(settings.general_noise_lvl)
      .scan<'i', int>();
  program.add_argument("-nDD", "--needleDropDuration")
      .help("The duration of the needle sound in 1s (at start of file)")
      .nargs(1)
      .default_value(settings.needle_drop_duration)
      .scan<'g', float>();
  program.add_argument("-nLD", "--needleLiftDuration")
      .help("The duration of the needle sound in 1s (at end of file)")
      .nargs(1)
      .default_value(settings.needle_lift_duration)
      .scan<'g', float>();

  // Check if arguments where passed correctly
  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  // The required arguments
  auto file = program.get<std::string>("Sourcepath");
  auto output_path = program.get<std::string>("Outputpath");

  /*
   * Fill settings with optional arguments if given
   *  else it uses the default values of the Settings struct
   */
  settings.sample_rate = program.get<uint32_t>("--samples");
  settings.bit_depth = program.get<uint16_t>("--bitDepth");
  settings.crackling_noise_lvl = program.get<uint16_t>("--cracklingNoiseLvl");
  settings.general_noise_lvl = program.get<uint16_t>("--generalNoiseLvl");
  settings.needle_drop_duration = program.get<float>("--needleDropDuration");
  settings.needle_lift_duration = program.get<float>("--needleLiftDuration");

  // Run main logic
  try {
    if (std::filesystem::is_directory(file)) {
      for (const auto &entry : std::filesystem::directory_iterator(file)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
          run_procedure(entry.path().string(), output_path, settings);
        }
      }
    } else {
      run_procedure(file, output_path, settings);
    }
  } catch (const char *error) {
    std::cerr << "Error: " << error << std::endl;
  }

  return 0;
}

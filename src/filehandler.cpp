#include "filehandler.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

void output_wav_data(WAVHeader &wav) {
  std::cout << "RIFF Header: " << std::string(wav.riff_header, 4) << std::endl;
  std::cout << "File Size: " << wav.wav_size << std::endl;
  std::cout << "WAVE Header: " << std::string(wav.wave_header, 4) << std::endl;
  std::cout << "FMT Header: " << std::string(wav.fmt_header, 4) << std::endl;
  std::cout << "FMT Chunk Size: " << wav.fmt_chunk_size << std::endl;
  std::cout << "Audio Format: " << wav.audio_format << std::endl;
  std::cout << "Number of Channels: " << wav.num_channels << std::endl;
  std::cout << "Sample Rate: " << wav.sample_rate << std::endl;
  std::cout << "Byte Rate: " << wav.byte_rate << std::endl;
  std::cout << "Block Align: " << wav.block_align << std::endl;
  std::cout << "Bits Per Sample: " << wav.bits_per_sample << std::endl;
  std::cout << "Data Header: " << std::string(wav.data_header, 4) << std::endl;
  std::cout << "Data Size: " << wav.data_size << std::endl;
  return;
}

WAVHeader read_wav_file(std::string file) {
  WAVHeader wav;

  // All checks for correct wav file
  std::ifstream wave_file(file, std::ios::binary);
  if (!wave_file) {
    throw "Failed to open input file.\n";
  }

  wave_file.read(wav.riff_header, 4);
  if (std::strncmp(wav.riff_header, "RIFF", 4) != 0) {
    throw "File seams to be currupted!\n";
  }

  wave_file.read(reinterpret_cast<char *>(&wav.wav_size), sizeof(wav.wav_size));

  wave_file.read(wav.wave_header, 4);
  if (std::strncmp(wav.wave_header, "WAVE", 4) != 0) {
    throw "Not a valid WAVE file.\n";
  }

  wave_file.read(wav.fmt_header, 4);
  if (std::strncmp(wav.fmt_header, "fmt ", 4) != 0) {
    throw "Missing 'fmt ' subchunk.\n";
  }

  wave_file.read(reinterpret_cast<char *>(&wav.fmt_chunk_size),
                 sizeof(wav.fmt_chunk_size));
  if (wav.fmt_chunk_size != 16) { // For PCM, the chunk size should be 16
    throw "Unexpected fmt chunk size.\n";
  }

  wave_file.read(reinterpret_cast<char *>(&wav.audio_format),
                 sizeof(wav.audio_format));
  if (wav.audio_format != 1) {
    throw "Unsupported audio format (only PCM is supported).\n";
  }

  wave_file.read(reinterpret_cast<char *>(&wav.num_channels),
                 sizeof(wav.num_channels));
  wave_file.read(reinterpret_cast<char *>(&wav.sample_rate),
                 sizeof(wav.sample_rate));
  wave_file.read(reinterpret_cast<char *>(&wav.byte_rate), sizeof(wav.byte_rate));
  wave_file.read(reinterpret_cast<char *>(&wav.block_align),
                 sizeof(wav.block_align));
  wave_file.read(reinterpret_cast<char *>(&wav.bits_per_sample),
                 sizeof(wav.bits_per_sample));
  if (wav.byte_rate !=
      wav.sample_rate * wav.num_channels * wav.bits_per_sample / 8) {
    throw "Byte rate seams to be wrong\n";
  } else if (wav.block_align != wav.num_channels * wav.bits_per_sample / 8) {
    throw "Block align seams to be wrong\n";
  }

  while (!wave_file.eof()) {
    wave_file.read(wav.data_header, 4);
    if (std::strncmp(wav.data_header, "data", 4) == 0) {
      break;
    }
    wave_file.seekg(-3, std::ios_base::cur);
  }

  wave_file.read(reinterpret_cast<char *>(&wav.data_size), sizeof(wav.data_size));

  wav.data.resize(wav.data_size);
  wave_file.read(reinterpret_cast<char *>(wav.data.data()), wav.data_size);
  if (!wave_file) {
    throw "Error reading the WAV file data.\n";
  }

  wave_file.close();

  return wav;
}

void write_wav_file(WAVHeader &wav, std::string filename) {
  std::ofstream out_file(filename, std::ios::binary);
  if (!out_file) {
    throw "Error creating new file\n";
  }

  // Write the WAV file header
  out_file.write(wav.riff_header, 4);
  out_file.write(reinterpret_cast<char *>(&wav.wav_size), sizeof(wav.wav_size));
  out_file.write(wav.wave_header, 4);
  out_file.write(wav.fmt_header, 4);
  out_file.write(reinterpret_cast<char *>(&wav.fmt_chunk_size),
                 sizeof(wav.fmt_chunk_size));
  out_file.write(reinterpret_cast<char *>(&wav.audio_format),
                 sizeof(wav.audio_format));
  out_file.write(reinterpret_cast<char *>(&wav.num_channels),
                 sizeof(wav.num_channels));
  out_file.write(reinterpret_cast<char *>(&wav.sample_rate),
                 sizeof(wav.sample_rate));
  out_file.write(reinterpret_cast<char *>(&wav.byte_rate), sizeof(wav.byte_rate));
  out_file.write(reinterpret_cast<char *>(&wav.block_align),
                 sizeof(wav.block_align));
  out_file.write(reinterpret_cast<char *>(&wav.bits_per_sample),
                 sizeof(wav.bits_per_sample));
  out_file.write(wav.data_header, 4);
  out_file.write(reinterpret_cast<char *>(&wav.data_size), sizeof(wav.data_size));

  // Write the audio data
  out_file.write(reinterpret_cast<char *>(wav.data.data()), wav.data_size);

  if (!out_file) {
    throw "Error writing to new file\n";
  }

  out_file.close();
  if (!out_file) {
    throw "Error closing new file\n";
  }
  return;
}

std::string base_name(std::filesystem::path const &path) {
  return path.filename();
}

std::string generate_file_name(std::filesystem::path path,
                               std::filesystem::path filename) {
  // check if path is a folder path that exists otherwise error
  if (!std::filesystem::is_directory(path.parent_path())) {
    throw "The given path is not a directory path or the directory doesn't "
          "exist.\n";
  }

  // check if filename is like *.wav otherwise error
  if (filename.extension() != ".wav") {
    throw "The file provided is no wav file.\n";
  }

  std::string generated_path = static_cast<std::string>(path.parent_path()) +
                               "/vinyl_" + static_cast<std::string>(filename);

  return generated_path;
}

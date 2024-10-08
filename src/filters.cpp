#include "filehandler.hpp"
#include <cstdint>
#include <iostream>
#include <vector>

// Limit bit depth

void limit_bit_depth(WAVHeader &audio, uint16_t new_bit_depth = 24) {
  // 16; 24; 32Bit possible
  if (new_bit_depth > audio.bitsPerSample) {
    std::cerr << "New bit depth is greater than current bit depth."
              << std::endl;
    return;
  }

  uint16_t original_bit_depth = audio.bitsPerSample;
  int bit_depth_difference = original_bit_depth - new_bit_depth;
  int max_value = (1 << (new_bit_depth - 1)) - 1;
  int min_value = -(1 << (new_bit_depth - 1));

  for (auto &sample : audio.data) {
    sample = std::clamp(sample >> bit_depth_difference, min_value, max_value);
    sample = sample << bit_depth_difference;
  }

  audio.bitsPerSample = new_bit_depth;
  audio.byteRate = audio.sampleRate * audio.numChannels * new_bit_depth / 8;
  audio.blockAlign = audio.numChannels * new_bit_depth / 8;

  return;
}

// Limit samping rate

void limit_sampling_rate(WAVHeader &audio, uint32_t new_sample_rate = 48000) {
  // Around 48000Hz
  if (new_sample_rate == audio.sampleRate) {
    std::cerr << "The new sample rate is the same as the current sample rate"
              << std::endl;
    return;
  }

  uint32_t old_sample_rate = audio.sampleRate;
  size_t old_num_samples = audio.data.size();
  size_t new_num_samples = static_cast<size_t>(
      (static_cast<double>(new_sample_rate) / old_sample_rate) *
      old_num_samples);

  std::vector<int16_t> new_data(new_num_samples);

  for (size_t i = 0; i < new_num_samples; ++i) {
    double old_index =
        static_cast<double>(i) * old_sample_rate / new_sample_rate;
    size_t index_floor = static_cast<size_t>(std::floor(old_index));
    size_t index_ceil = std::min(index_floor + 1, old_num_samples - 1);

    double fraction = old_index - index_floor;
    new_data[i] =
        static_cast<int16_t>((1.f - fraction) * audio.data[index_floor] +
                             fraction * audio.data[index_ceil]);
  }

  audio.data = std::move(new_data);

  audio.sampleRate = new_sample_rate;
  audio.byteRate =
      new_sample_rate * audio.numChannels * audio.bitsPerSample / 8;
  audio.blockAlign = audio.numChannels * audio.bitsPerSample / 8;
  audio.dataSize = audio.data.size() * sizeof(int16_t);
  audio.wavSize = audio.dataSize + sizeof(WAVHeader) - 8;

  return;
}

// Limit the dynamic range

inline double dBToLinear(double dB) {
  // 20*dB because doubling the amplitude is 6dB (10^1/20 = 2).
  return std::pow(10.0, dB / 20.0);
}

void limit_dynamic_range(WAVHeader &audio,
                         std::vector<double> dynamic_range = {55.0, 65.0}) {
  // Vinyl has a dynamic range of 55dB-65dB
  if (dynamic_range.size() != 2) {
    throw "dynamic_range should contain exactly 2 values: min and max";
  }

  double min_dB = dynamic_range[0];
  double max_dB = dynamic_range[1];

  if (min_dB > max_dB) {
    throw "The first value of dynamic_range must be less than or equal to the "
          "second value";
  }

  double max_sample_value = std::pow(2, audio.bitsPerSample - 1) - 1;
  double min_sample_value = -std::pow(2, audio.bitsPerSample - 1);

  int16_t min_val = static_cast<int16_t>(min_sample_value * dBToLinear(min_dB));
  int16_t max_val = static_cast<int16_t>(max_sample_value * dBToLinear(max_dB));

  for (auto &sample : audio.data) {
    sample = std::clamp(sample, min_val, max_val);
  }

  return;
}

// Adding Noise to the struct

inline int16_t generate_crackle_noise_value() { return (rand() % 2000) - 1000; }

inline int16_t generate_pop_click_noise_value() {
  return (rand() % 2 ? -16384 : 16384);
}

void add_crackle_noise(WAVHeader &audio, int noise_level = 0) {
  srand(static_cast<unsigned int>(time(0)));

  noise_level = (noise_level < 0)      ? 0
                : (noise_level > 1000) ? 1000
                                       : noise_level;

  if (noise_level == 0) {
    return;
  }

  for (int i = 0; i < static_cast<int>(audio.data.size());
       i += audio.blockAlign) {
    if (rand() % 1000 < noise_level) {
      short *sample = reinterpret_cast<short *>(&audio.data[i]);
      int16_t crackle_noise = generate_crackle_noise_value();
      *sample = std::min(
          std::max(static_cast<int>(*sample) + crackle_noise, -32768), 32767);
    }
  }
}

void add_pop_click_noise(WAVHeader &audio, int noise_level = 0) {
  srand(static_cast<unsigned int>(time(0)));

  noise_level = (noise_level < 0)      ? 0
                : (noise_level > 1000) ? 1000
                                       : noise_level;

  if (noise_level == 0) {
    return;
  }

  for (int i = 0; i < static_cast<int>(audio.data.size());
       i += audio.blockAlign) {
    if (rand() % 10000 < noise_level) {
      short *sample = reinterpret_cast<short *>(&audio.data[i]);
      int16_t pop_click_noise = generate_pop_click_noise_value();
      *sample = std::min(
          std::max(static_cast<int>(*sample) + pop_click_noise, -16384), 16384);
    }
  }
}

// Add needle sounds to the struct

void add_start_needle(WAVHeader &audio, int additional_param) {
  // some logic here
  std::cout << audio.byteRate << additional_param << std::endl;
  return;
}

void add_end_needle(WAVHeader &audio, int additional_param) {
  // some logic here
  std::cout << audio.byteRate << additional_param << std::endl;
  return;
}

// Calculate and shorten audio length

double calc_audio_length(const WAVHeader &audio) {
  uint32_t number_of_samples =
      audio.dataSize / (audio.numChannels * (audio.bitsPerSample / 8));
  double duration = static_cast<double>(number_of_samples) / audio.sampleRate;
  return duration;
}

void shorten_audio(WAVHeader &audio, double audio_length) {
  uint32_t desired_samples =
      static_cast<uint32_t>(audio_length * audio.sampleRate);
  uint32_t samples_per_channel = desired_samples * audio.numChannels;

  uint32_t total_current_samples =
      audio.dataSize / (audio.numChannels * (audio.bitsPerSample / 8));
  if (desired_samples > total_current_samples) {
    std::cerr << "Desired length exceeds the current length of the audio."
              << std::endl;
    return;
  }

  // Resize the data vector to hold the samples for the desired length
  audio.data.resize(samples_per_channel);

  // Update the dataSize in the header to reflect the new size of the audio data
  audio.dataSize = samples_per_channel * (audio.bitsPerSample / 8);

  // Update the wavSize in the header to reflect the new size of the entire file
  audio.wavSize = audio.dataSize + sizeof(WAVHeader) -
                  8; // Subtract 8 for the 'RIFF' and size fields
  return;
}

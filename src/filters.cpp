#include "filehandler.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

// Limit bit depth (same as dynamic limiting the dynamic range)

void limit_bit_depth(WAVHeader &audio, const uint16_t &new_bit_depth) {
  // 16; 24; 32Bit possible
  // resize instead of limit????
  if (new_bit_depth > audio.bits_per_sample) {
    std::cerr << "New bit depth is greater than current bit depth.\n";
    return;
  }

  uint16_t original_bit_depth = audio.bits_per_sample;
  int bit_depth_difference = original_bit_depth - new_bit_depth;
  int max_value = (1 << (new_bit_depth - 1)) - 1;
  int min_value = -(1 << (new_bit_depth - 1));

  for (auto &sample : audio.data) {
    sample = std::clamp(sample >> bit_depth_difference, min_value, max_value);
    sample = sample << bit_depth_difference;
  }

  return;
}

// Adjust samping rate

void adjust_sampling_rate(WAVHeader &audio, const uint32_t &new_sample_rate) {
  // Around 48000Hz
  if (new_sample_rate == audio.sample_rate) {
    std::cerr << "The new sample rate is the same as the current sample rate\n";
    return;
  }

  uint32_t old_sample_rate = audio.sample_rate;
  size_t old_num_samples = audio.data_size;
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

  audio.sample_rate = new_sample_rate;
  audio.byte_rate =
      new_sample_rate * audio.num_channels * audio.bits_per_sample / 8;
  audio.block_align = audio.num_channels * audio.bits_per_sample / 8;
  audio.data_size = audio.data.size() * sizeof(int16_t);
  audio.wav_size = audio.data_size + sizeof(WAVHeader) - 8;

  return;
}

// Adding Noise to the struct

inline int16_t generate_crackle_noise_value() {
  return ((rand() % 2000) - 1000) * 2;
}

inline int16_t generate_pop_click_noise_value() {
  return (rand() % 2 ? -16384 : 16384);
}

void add_crackle_noise(WAVHeader &audio, const uint16_t &noise_level) {
  srand(static_cast<unsigned int>(time(0)));

  if (noise_level > 10000) {
    throw "noise_level can not be greater than 10_000 aka 100%\n";
  }

  if (noise_level == 0) {
    return;
  }

  for (int i = 0; i < static_cast<int>(audio.data_size);
       i += audio.block_align) {
    if (rand() % 10000 < noise_level) {
      short *sample = reinterpret_cast<short *>(&audio.data[i]);
      int16_t crackle_noise = generate_crackle_noise_value();
      *sample = std::min(
          std::max(static_cast<int>(*sample) + crackle_noise, -32768), 32767);
    }
  }

  return;
}

void add_pop_click_noise(WAVHeader &audio, const uint32_t &noise_level) {
  srand(static_cast<unsigned int>(time(0)));

  if (noise_level > 100000l) {
    throw "noise_level can not be greater than 10_000 aka 100%\n";
  }

  if (noise_level == 0) {
    return;
  }

  for (int i = 0; i < static_cast<int>(audio.data_size);
       i += audio.block_align) {
    if (rand() % 100000l < noise_level) {
      short *sample = reinterpret_cast<short *>(&audio.data[i]);
      int16_t pop_click_noise = generate_pop_click_noise_value();
      *sample = std::min(
          std::max(static_cast<int>(*sample) + pop_click_noise, -16384), 16384);
    }
  }

  return;
}

// Add needle sounds to the struct

std::vector<int16_t> generate_needle_sound(const int &sample_rate,
                                           const int &num_channels,
                                           const float &duration_seconds) {
  std::vector<int16_t> sound;

  if (duration_seconds <= 0) {
    return sound;
  }

  size_t numSamples = static_cast<size_t>(duration_seconds * sample_rate);
  size_t impactSamples = static_cast<size_t>(0.01 * sample_rate); // 10ms impact
  size_t frictionSamples = numSamples - impactSamples;

  std::default_random_engine generator;
  std::uniform_int_distribution<int16_t> noiseDistribution(-25000, -23000);

  // Generate initial impact sound (short burst of loud noise)
  for (size_t i = 0; i < impactSamples; ++i) {
    int16_t sampleValue = noiseDistribution(generator);
    for (uint16_t channel = 0; channel < num_channels; ++channel) {
      sound.push_back(sampleValue);
    }
  }

  // Generate friction noise
  for (size_t i = 0; i < frictionSamples; ++i) {
    double decay = exp(-static_cast<double>(i) /
                       (frictionSamples / 2.0 * duration_seconds));
    int16_t sampleValue =
        static_cast<int16_t>(noiseDistribution(generator) * decay);
    for (uint16_t channel = 0; channel < num_channels; ++channel) {
      sound.push_back(sampleValue);
    }
  }

  return sound;
}

void add_start_needle(WAVHeader &audio, const float &needle_drop_duration) {

  if (needle_drop_duration < 0) {
    throw "The needle_drop_duration can not be less than 0\n";
  }

  // Generate the needle drop sound.
  std::vector<int16_t> needle_drop_sound = generate_needle_sound(
      audio.sample_rate, audio.num_channels, needle_drop_duration);

  // Create a new buffer to hold the combined audio data.
  std::vector<int16_t> newAudioData;
  newAudioData.reserve(needle_drop_sound.size() + audio.data_size);

  // Append needle drop sound.
  newAudioData.insert(newAudioData.end(), needle_drop_sound.begin(),
                      needle_drop_sound.end());

  // Append original audio data.
  newAudioData.insert(newAudioData.end(), audio.data.begin(), audio.data.end());

  // Update the audio data with the new combined data.
  audio.data = std::move(newAudioData);

  if (audio.bits_per_sample < 16) {
    audio.bits_per_sample = 16;
  }

  audio.data_size = audio.data.size() * sizeof(int16_t);
  audio.wav_size = audio.data_size + sizeof(WAVHeader) - 8;

  return;
}

void add_end_needle(WAVHeader &audio, const float &needle_lift_duration) {

  if (needle_lift_duration < 0) {
    throw "The needle_lift_duration can not be less than 0\n";
  }

  // Generate the needle lift sound.
  auto needle_lift_sound = generate_needle_sound(
      audio.sample_rate, audio.num_channels, needle_lift_duration);

  // Append needle lift sound to the end of the audio data.
  audio.data.insert(audio.data.end(), needle_lift_sound.begin(),
                    needle_lift_sound.end());

  if (audio.bits_per_sample < 16) {
    audio.bits_per_sample = 16;
  }

  audio.data_size = audio.data.size() * sizeof(int16_t);
  audio.wav_size = audio.data_size + sizeof(WAVHeader) - 8;

  return;
}

// Calculate and resize audio length

double calc_audio_length(const WAVHeader &audio) {
  uint32_t number_of_samples =
      audio.data_size / (audio.num_channels * (audio.bits_per_sample / 8));
  double duration = static_cast<double>(number_of_samples) / audio.sample_rate;
  return duration;
}

void resize_audio(WAVHeader &audio, const double &audio_length) {
  uint32_t desired_samples =
      static_cast<uint32_t>(audio_length * audio.sample_rate);
  uint32_t samples_per_channel = desired_samples * audio.num_channels;

  // Resize the data vector to hold the samples for the desired length
  audio.data.resize(samples_per_channel);

  /* Update the data_size in the header to reflect the new size of the audio
   data */
  audio.data_size = samples_per_channel * (audio.bits_per_sample / 8);

  /* Update the wav_size in the header to reflect the new size of the entire
   file */
  audio.wav_size = audio.data_size + sizeof(WAVHeader) -
                   8; // Subtract 8 for the 'RIFF' and size fields
  return;
}

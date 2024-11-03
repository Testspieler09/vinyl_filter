#ifndef FILTERS_H
#define FILTERS_H
#include "filehandler.hpp"
#include <cstdint>

/**
 * The default settings for the vinyl filter.
 */
struct Settings {
  uint32_t sample_rate = 48000;       // in 1Hz
  uint16_t bit_depth = 24;            // in 1Bit
  uint16_t crackling_noise_lvl = 100; // in 0.01%
  uint16_t general_noise_lvl = 5;     // in 0.001%
  float needle_drop_duration = 0.8f;  // in 1s
  float needle_lift_duration = 1.f;   // in 1s
};

/**
 * A function that calculates the length of a audiofile in seconds.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @return duration The length of the file in seconds
 */
double calc_audio_length(const WAVHeader &audio);

/**
 * A function that limits the bit depth to a given value
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] bit_depth The new bit depth
 */
void limit_bit_depth(WAVHeader &audio, const uint16_t &bit_depth);

/**
 * A function that adjusts the sampling rate of the given audio file to a given
 * rate.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] sampling_rate The new sampling rate
 */
void adjust_sampling_rate(WAVHeader &audio, const uint32_t &sampling_rate);

/**
 * A function that adds crackle noises based on the given parameters.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] noise_level The amount of noise generated (1 -> 0.1%)
 */
void add_crackle_noise(WAVHeader &audio, const uint16_t &noise_level);

/**
 * A function that adds pop noises based on the given parameters.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] noise_level The amount of noise generated (1 -> 0.01%)
 */
void add_pop_click_noise(WAVHeader &audio, const uint32_t &noise_level);

/**
 * A function that adds the sound of the needle dropping on the vinyl record
 * based on the given parameters at the start of the file.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] needle_drop_duration The duration of the generated sound effect in
 * 1s
 */
void add_start_needle(WAVHeader &audio, const float &needle_drop_duration);

/**
 * A function that adds the sound of the needle dropping on the vinyl record
 * based on the given parameters at the end of the file.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] needle_lift_duration The duration of the generated sound effect in
 * 1s
 */
void add_end_needle(WAVHeader &audio, const float &needle_lift_duration);

/**
 * A function that shortens the audiofile to a given length in seconds.
 *
 * @param[out] audio The audio file read into the WAVHeader struct
 * @param[in] audio_length The length of the audiofile in seconds
 */
void resize_audio(WAVHeader &audio, const double &audio_length);

#endif

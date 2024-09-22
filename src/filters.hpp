#ifndef FILTERS_H
#define FILTERS_H
#include "filehandler.hpp"
#include <cstdint>
#include <vector>

double calc_audio_length(WAVHeader& audio);
void limit_bit_depth(WAVHeader& audio, uint16_t bit_depth=24);
void limit_sampling_rate(WAVHeader& audio, uint32_t sampling_rate=48000);
void limit_dynamic_range(WAVHeader& audio, std::vector<int16_t> dynamic_range);
void add_noise(WAVHeader& audio, int additional_param);
void add_start_needle(WAVHeader& audio, int additional_param);
void add_end_needle(WAVHeader& audio, int additional_param);
void shorten_audio(WAVHeader& audio, double audio_length);

#endif

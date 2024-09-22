#ifndef FILTERS_H
#define FILTERS_H
#include "filehandler.hpp"
#include <cstdint>
#include <vector>

/**
* A function that calculates the length of a audiofile in seconds.
* @param[out]	audio		The audio file read into the WAVHeader struct
* @return	duration	The length of the file in seconds
*/
double calc_audio_length(WAVHeader& audio);

/**
* A function that limits the bit depth to a given value
* @param[out]	audio		The audio file read into the WAVHeader struct
* @param[in]	bit_depth	The new bit depth
*/
void limit_bit_depth(WAVHeader& audio, uint16_t bit_depth=24);

/**
* A function that limits the sampling rate of the given audio file to a given rate.
* @param[out]	audio		The audio file read into the WAVHeader struct
* @param[in]	sampling_rate	The new sampling rate
*/
void limit_sampling_rate(WAVHeader& audio, uint32_t sampling_rate=48000);

/**
* A function that limits the dynamic range of the given audio file to a given range.
* @param[out]	audio		The audio file read into the WAVHeader struct
* @param[in]	dynamic_range	The new range as a std::vector<int16_t>
*/
void limit_dynamic_range(WAVHeader& audio, std::vector<int16_t> dynamic_range);

/**
* A function that adds noise based on the given parameters.
* @param[out]	audio			The audio file read into the WAVHeader struct
* @param[in]	additional_param	Something not defined yet
*/
void add_noise(WAVHeader& audio, int additional_param);

/**
* A function that adds the sound of the needle dropping on the vinyl record based on the given parameters at the start of the file.
* @param[out]	audio			The audio file read into the WAVHeader struct
* @param[in]	additional_param	Something not defined yet
*/
void add_start_needle(WAVHeader& audio, int additional_param);

/**
* A function that adds the sound of the needle dropping on the vinyl record based on the given parameters at the end of the file.
* @param[out]	audio			The audio file read into the WAVHeader struct
* @param[in]	additional_param	Something not defined yet
*/
void add_end_needle(WAVHeader& audio, int additional_param);

/**
* A function that shortens the audiofile to a given length in seconds.
* @param[out]	audio		The audio file read into the WAVHeader struct
* @param[in]	audio_length	The length of the audiofile in seconds
*/
void shorten_audio(WAVHeader& audio, double audio_length);

#endif

#include "filehandler.hpp"
#include <iostream>
#include <cstdint>
#include <vector>

void limit_bit_depth(WAVHeader& audio, uint16_t new_bit_depth=24) {
	// 16; 24; 32Bit possible
	if (new_bit_depth > audio.bitsPerSample) {
		std::cerr << "New bit depth is greater than current bit depth." << std::endl;
		return;
	}

	uint16_t original_bit_depth = audio.bitsPerSample;
	int bit_depth_difference = original_bit_depth - new_bit_depth;
	int max_value = (1 << (new_bit_depth - 1)) - 1;
	int min_value = -(1 << (new_bit_depth - 1));

	for (auto& sample : audio.data) {
		sample = std::clamp(sample >> bit_depth_difference, min_value, max_value);
		sample = sample << bit_depth_difference;
	}

	audio.bitsPerSample = new_bit_depth;
	audio.byteRate = audio.sampleRate * audio.numChannels * new_bit_depth / 8;
	audio.blockAlign = audio.numChannels * new_bit_depth / 8;

	return;
}

void limit_sampling_rate(WAVHeader& audio, uint32_t new_sample_rate=48000) {
	// Around 48000Hz
	if (new_sample_rate == audio.sampleRate) {
		std::cerr << "The new sample rate is the same as the current sample rate" << std::endl;
		return;
	}

	uint32_t old_sample_rate = audio.sampleRate;
	size_t old_num_samples = audio.data.size();
	size_t new_num_samples = static_cast<size_t>(
		(static_cast<double>(new_sample_rate) / old_sample_rate) * old_num_samples);

	std::vector<int16_t> new_data(new_num_samples);

	for (size_t i = 0; i < new_num_samples; ++i) {
		double old_index = static_cast<double>(i) * old_sample_rate / new_sample_rate;
		size_t index_floor = static_cast<size_t>(std::floor(old_index));
		size_t index_ceil = std::min(index_floor + 1, old_num_samples - 1);

		double fraction = old_index - index_floor;
		new_data[i] = static_cast<int16_t>(
			(1.f - fraction) * audio.data[index_floor] +
			fraction * audio.data[index_ceil]
		);
	}

	audio.data = std::move(new_data);

	audio.sampleRate = new_sample_rate;
	audio.byteRate = new_sample_rate * audio.numChannels * audio.bitsPerSample / 8;
	audio.blockAlign = audio.numChannels * audio.bitsPerSample / 8;
	audio.dataSize = audio.data.size() * sizeof(int16_t);
	audio.wavSize = audio.dataSize + sizeof(WAVHeader) - 8;

	return;
}

void limit_dynamic_range(WAVHeader& audio, std::vector<int16_t> dynamic_range) {
	// Vinyl has a dynamic range of 55dB-65dB
	return;
}

void add_noise(WAVHeader& audio, int additional_param) {
	// some logic here
	return;
}

void add_start_needle(WAVHeader& audio, int additional_param) {
	// some logic here
	return;
}

void add_end_needle(WAVHeader& audio, int additional_param) {
	// some logic here
	return;
}

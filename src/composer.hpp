#pragma once
#include <cstdint>
#include <vector>
#include "audio_types.hpp"
namespace orchard {
constexpr int sampleRate=22050;
struct PCM { std::vector<std::int16_t> samples; };
PCM composeMusic();
PCM composeEffect(Cue cue);
std::vector<unsigned char> asWav(const PCM& pcm);
}

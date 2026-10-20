#pragma once
namespace orchard {
enum class Cue { Eat, Click, Countdown, Lose, Win, Count };
inline constexpr const char* cueFiles[]={"audio/eat.wav","audio/click.wav","audio/countdown.wav","audio/lose.wav","audio/win.wav"};
}

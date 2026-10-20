#pragma once
#include "audio_types.hpp"
#include "asset_files.hpp"
#include "raylib.h"
#include <array>
namespace orchard {
class Audio {
public:
    bool init(const AssetFiles& files);
    void update(float dt);
    void effect(Cue cue);
    void setMusic(float volume);
    void setEffects(float volume);
    void duck(bool value) { ducked=value; }
    void shutdown();
    bool ready() const { return active; }
    bool mediaLoaded() const { return loaded; }
private:
    bool active=false, ducked=false,loaded=false;
    float musicVolume=.48f, effectsVolume=.65f, gain=.48f;
    Music track{};
    std::vector<unsigned char> wav;
    std::array<Sound,static_cast<int>(Cue::Count)> effects{};
};
}

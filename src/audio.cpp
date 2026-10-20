#include "audio.hpp"
#include <algorithm>
namespace orchard {
bool Audio::init(const AssetFiles& files) {
    InitAudioDevice(); active=IsAudioDeviceReady(); if(!active) return false;
    wav=files.read("audio/orchard-loop.wav");
    if(wav.empty()) return false;
    track=LoadMusicStreamFromMemory(".wav",wav.data(),static_cast<int>(wav.size()));
    if(IsMusicValid(track)) { track.looping=true; SetMusicVolume(track,musicVolume); PlayMusicStream(track); }
    loaded=IsMusicValid(track);
    for(int i=0;i<static_cast<int>(Cue::Count);i++) {
        auto bytes=files.read(cueFiles[i]);
        if(bytes.empty()) {loaded=false;continue;}
        Wave wave=LoadWaveFromMemory(".wav",bytes.data(),static_cast<int>(bytes.size()));
        if(!IsWaveValid(wave)) {loaded=false;continue;}
        effects[i]=LoadSoundFromWave(wave);
        UnloadWave(wave);loaded=loaded&&IsSoundValid(effects[i]);
    }
    return loaded;
}
void Audio::update(float dt) {
    if(!active) return;
    gain+=(musicVolume*(ducked?.35f:1.f)-gain)*std::min(1.f,dt*5.f);
    if(IsMusicValid(track)) { SetMusicVolume(track,gain); UpdateMusicStream(track); }
}
void Audio::effect(Cue cue) {
    if(!active) return;
    Sound s=effects[static_cast<int>(cue)];
    if(IsSoundValid(s)) { SetSoundVolume(s,effectsVolume); PlaySound(s); }
}
void Audio::setMusic(float v) { musicVolume=std::clamp(v,0.f,1.f); }
void Audio::setEffects(float v) { effectsVolume=std::clamp(v,0.f,1.f); }
void Audio::shutdown() {
    if(!active) return;
    for(auto s:effects) if(IsSoundValid(s)) UnloadSound(s);
    if(IsMusicValid(track)) UnloadMusicStream(track);
    CloseAudioDevice(); active=false;
}
}

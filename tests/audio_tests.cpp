#include "composer.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace orchard;
int main() {
    PCM music=composeMusic();
    if(music.samples.size()<sampleRate*2*30 || music.samples.size()>sampleRate*2*40) {
        std::cerr<<"FAIL: original loop must contain 30-40 seconds of stereo music\n"; return 1;
    }
    double energy=0; int peak=0; bool stereo=false;
    for(std::size_t i=0;i<music.samples.size();i++) {
        int v=std::abs(static_cast<int>(music.samples[i])); peak=std::max(peak,v); energy+=double(v)*v;
        if(i%2 && music.samples[i]!=music.samples[i-1]) stereo=true;
    }
    double rms=std::sqrt(energy/music.samples.size())/32768.;
    if(peak>=31000 || rms<.025 || rms>.4 || !stereo) { std::cerr<<"FAIL: music must be audible stereo without clipping\n"; return 1; }
    auto bytes=asWav(music);
    if(bytes.size()!=44+music.samples.size()*2 || bytes[0]!='R' || bytes[8]!='W') { std::cerr<<"FAIL: valid PCM WAV export\n"; return 1; }
    for(int i=0;i<static_cast<int>(Cue::Count);i++) {
        auto s=composeEffect(static_cast<Cue>(i));
        if(s.samples.empty() || std::all_of(s.samples.begin(),s.samples.end(),[](auto x){return x==0;})) {
            std::cerr<<"FAIL: each game event needs audible feedback\n"; return 1;
        }
    }
    std::cout<<"Original music: "<<music.samples.size()/2./sampleRate<<" seconds, RMS "<<rms<<", peak "<<peak<<"; all five effects audible\n";
}

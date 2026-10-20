#include "composer.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
namespace orchard {
namespace {
constexpr double tau=6.283185307179586;
double hz(int midi) { return 440.*std::pow(2.,(midi-69)/12.); }
using Mix=std::vector<float>;
void note(Mix& mix,double at,double duration,int midi,double volume,double pan,int voice) {
    int total=static_cast<int>(mix.size()/2), start=static_cast<int>(at*sampleRate);
    int length=static_cast<int>(duration*sampleRate);
    double f=hz(midi);
    for(int i=0;i<length;i++) {
        double t=i/double(sampleRate), phase=tau*f*t;
        double attack=std::min(1.,t/.007), release=std::min(1.,(duration-t)/.045);
        double v;
        if(voice==0) v=(std::sin(phase)+.3*std::sin(phase*2.003)+.13*std::sin(phase*3.99))*std::exp(-t*7.5);
        else if(voice==1) v=(std::sin(phase)+.22*std::sin(phase*2))*std::exp(-t*3.7);
        else v=(std::sin(phase)+.18*std::sin(phase*3))*std::exp(-t*2.8);
        v*=volume*attack*release;
        int frame=(start+i)%total;
        mix[frame*2]+=static_cast<float>(v*(1.-pan*.5));
        mix[frame*2+1]+=static_cast<float>(v*(1.+pan*.5));
    }
}
void drum(Mix& mix,double at,int type,double volume) {
    int length=static_cast<int>(sampleRate*(type==0?.22:.12)),total=static_cast<int>(mix.size()/2);
    unsigned rng=7331;
    double phase=0;
    for(int i=0;i<length;i++) {
        double t=i/double(sampleRate);
        rng=rng*1664525u+1013904223u;
        double noise=(double(rng)/4294967295.)*2.-1.;
        phase+=tau*(48.+95.*std::exp(-t*35.))/sampleRate;
        double v=type==0?std::sin(phase)*std::exp(-t*22.):noise*std::exp(-t*(type==1?45.:85.));
        if(type==1) v=.65*v+.35*std::sin(tau*180.*t)*std::exp(-t*30.);
        int p=(static_cast<int>(at*sampleRate)+i)%total;
        mix[p*2]+=static_cast<float>(v*volume);
        mix[p*2+1]+=static_cast<float>(v*volume*(type==2?.65:1.));
    }
}
PCM finish(const Mix& mix) {
    PCM out; out.samples.reserve(mix.size());
    for(float value:mix) out.samples.push_back(static_cast<std::int16_t>(std::tanh(value)*26000.));
    return out;
}
}
PCM composeMusic() {
    constexpr double beat=60./112.;
    Mix mix(static_cast<std::size_t>(16*4*beat*sampleRate)*2,0.f);
    const int roots[4]={48,53,57,55};
    const int chords[4][4]={{60,64,67,71},{60,65,69,72},{60,64,69,72},{59,62,67,74}};
    const int melody[8][8]={
        {76,-1,79,81,79,76,74,-1},{72,76,79,-1,76,-1,74,72},
        {77,-1,81,84,81,79,77,-1},{76,77,79,-1,77,76,72,-1},
        {76,-1,81,84,83,81,79,76},{74,76,79,-1,76,-1,72,-1},
        {74,-1,79,83,81,79,74,71},{72,74,76,79,76,74,72,-1}
    };
    for(int bar=0;bar<16;bar++) {
        double t=bar*4*beat; int chord=(bar/2)%4;
        for(int b=0;b<4;b++) {
            note(mix,t+b*beat,.42,roots[chord]+(b==2?7:0),.19,0,1);
            drum(mix,t+b*beat,b%2?1:0,b%2?.045:.13);
            for(int eighth=0;eighth<2;eighth++) drum(mix,t+(b+eighth*.5)*beat,2,eighth?.025:.018);
            for(int n=0;n<4;n++) note(mix,t+(b+.5)*beat,.21,chords[chord][n],.026,(n-1.5)*.4,2);
        }
        for(int n=0;n<8;n++) {
            int m=melody[bar%8][n];
            if(m>=0) note(mix,t+n*.5*beat,.47,m,.125,bar%2?.25:-.25,0);
        }
        if(bar>=8) for(int n=0;n<4;n++) note(mix,t+(n+.25)*beat,.25,chords[chord][n]+12,.026,.6,0);
    }
    Mix dry=mix;
    for(std::size_t i=0;i<mix.size()/2;i++) for(int c=0;c<2;c++) {
        std::size_t size=mix.size()/2;
        auto a=(i+size-static_cast<int>(.21*sampleRate))%size;
        auto b=(i+size-static_cast<int>(.37*sampleRate))%size;
        mix[i*2+c]+=dry[a*2+1-c]*.13f+dry[b*2+c]*.07f;
    }
    return finish(mix);
}
PCM composeEffect(Cue cue) {
    double duration=cue==Cue::Lose?.75:cue==Cue::Win?1.3:.24;
    Mix mix(static_cast<std::size_t>(duration*sampleRate)*2,0.f);
    switch(cue) {
        case Cue::Eat: note(mix,0,.13,79,.4,-.1,0); note(mix,.065,.16,86,.3,.1,0); break;
        case Cue::Click: note(mix,0,.075,72,.18,0,0); break;
        case Cue::Countdown: note(mix,0,.12,76,.22,0,0); break;
        case Cue::Lose: note(mix,0,.26,67,.28,0,2); note(mix,.16,.25,63,.25,0,2); note(mix,.34,.4,55,.25,0,1); break;
        case Cue::Win: for(int i=0;i<4;i++) note(mix,i*.16,.5,72+(i==3?12:i*4),.25,(i-1.5)*.2,0); break;
        default: break;
    }
    return finish(mix);
}
std::vector<unsigned char> asWav(const PCM& pcm) {
    std::vector<unsigned char> out;
    auto str=[&](const char* p){for(int i=0;i<4;i++) out.push_back(static_cast<unsigned char>(p[i]));};
    auto u16=[&](unsigned v){out.push_back(v&255);out.push_back((v>>8)&255);};
    auto u32=[&](unsigned v){u16(v&65535);u16(v>>16);};
    unsigned bytes=static_cast<unsigned>(pcm.samples.size()*2);
    str("RIFF");u32(36+bytes);str("WAVE");str("fmt ");u32(16);u16(1);u16(2);
    u32(sampleRate);u32(sampleRate*4);u16(4);u16(16);str("data");u32(bytes);
    for(auto sample:pcm.samples) u16(static_cast<std::uint16_t>(sample));
    return out;
}
}

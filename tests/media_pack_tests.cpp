#include "asset_files.hpp"
#include "audio_types.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace orchard;
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) return 2;
    SetTraceLogLevel(LOG_WARNING);AssetFiles assets{std::filesystem::path(argv[1])};
    int errors=0;
    for(const auto& name:assets.missing()) {std::cerr<<"Missing asset: "<<name<<'\n';errors++;}
    const char* images[]={"mascot","sprites","backdrop","grass-board","ui-card","ui-button","snake-skin"};
    for(const auto* name:images) {
        auto path="images/"+std::string(name)+".png";auto bytes=assets.read(path.c_str());
        Image image=LoadImageFromMemory(".png",bytes.data(),static_cast<int>(bytes.size()));
        if(!IsImageValid(image)) {std::cerr<<"Invalid PNG: "<<path<<'\n';errors++;continue;}
        if(std::string(name)=="mascot"||std::string(name)=="sprites") {
            auto pixels=LoadImageColors(image);int transparent=0,opaque=0;
            for(int i=0;i<image.width*image.height;i++) {if(pixels[i].a==0) transparent++;if(pixels[i].a>240) opaque++;}
            if(transparent<image.width*image.height/5||opaque<image.width*image.height/10) {
                std::cerr<<"Sprite artwork must have genuine transparency and visible content: "<<name<<'\n';errors++;
            }
            if(std::string(name)=="sprites") {
                int cross=0,total=0;
                for(int grid=1;grid<4;grid++) {
                    int x=image.width*grid/4,y=image.height*grid/4;
                    for(int i=0;i<image.height;i++) {cross+=pixels[i*image.width+x].a>150;total++;}
                    for(int i=0;i<image.width;i++) {cross+=pixels[y*image.width+i].a>150;total++;}
                }
                if(cross>total/100) {std::cerr<<"Atlas sprites cross equal-cell boundaries: "<<cross<<" / "<<total<<'\n';errors++;}
            }
            UnloadImageColors(pixels);
        }
        UnloadImage(image);
    }
    for(int i=-1;i<static_cast<int>(Cue::Count);i++) {
        const char* path=i<0?"audio/orchard-loop.wav":cueFiles[i];auto bytes=assets.read(path);
        Wave wave=LoadWaveFromMemory(".wav",bytes.data(),static_cast<int>(bytes.size()));
        if(!IsWaveValid(wave)) {std::cerr<<"Invalid WAV: "<<path<<'\n';errors++;continue;}
        auto samples=LoadWaveSamples(wave);double energy=0;float peak=0;
        for(unsigned j=0;j<wave.frameCount*wave.channels;j++) {energy+=samples[j]*samples[j];peak=std::max(peak,std::abs(samples[j]));}
        if(peak>=.96f||peak<.05f||energy<=0||wave.channels!=2) {std::cerr<<"Silent, clipping or non-stereo audio: "<<path<<'\n';errors++;}
        double seconds=wave.frameCount/double(wave.sampleRate);
        if(i<0&&(seconds<30||seconds>40)) {std::cerr<<"Wrong soundtrack duration\n";errors++;}
        UnloadWaveSamples(samples);UnloadWave(wave);
    }
    const int turkish[]={199,231,286,287,304,305,214,246,350,351,220,252};
    for(const char* font:{"fonts/BalsamiqSans-Regular.ttf","fonts/BalsamiqSans-Bold.ttf"}) {
        auto bytes=assets.read(font);
        int glyphs[12];std::copy(std::begin(turkish),std::end(turkish),glyphs);
        auto data=LoadFontData(bytes.data(),static_cast<int>(bytes.size()),32,glyphs,12,FONT_DEFAULT);
        if(!data) {std::cerr<<"Invalid font\n";errors++;continue;}
        for(int i=0;i<12;i++) if(data[i].image.width<=0||data[i].advanceX<=0) {std::cerr<<"Missing Turkish glyph "<<glyphs[i]<<'\n';errors++;}
        UnloadFontData(data,12);
    }
    std::cout<<"External pack: 7 PNGs, 6 WAVs, 2 TTFs; "<<errors<<" errors\n";
    return errors?1:0;
}

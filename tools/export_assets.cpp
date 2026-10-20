// Explicit offline authoring tool. The shipped game never calls this code.
#include "composer.hpp"
#include "raylib.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
using namespace orchard;
namespace fs=std::filesystem;
namespace {
void png(const fs::path& path,int width,int height,const std::function<void()>& paint) {
    auto target=LoadRenderTexture(width,height);
    BeginTextureMode(target);ClearBackground(BLANK);paint();EndTextureMode();
    Image image=LoadImageFromTexture(target.texture);ImageFlipVertical(&image);
    int size=0;unsigned char* bytes=ExportImageToMemory(image,".png",&size);
    std::ofstream file(path,std::ios::binary);file.write(reinterpret_cast<char*>(bytes),size);
    MemFree(bytes);UnloadImage(image);UnloadRenderTexture(target);
    if(!file) throw std::runtime_error("Could not export PNG");
}
void wav(const fs::path& path,PCM pcm) {
    auto bytes=asWav(pcm);std::ofstream file(path,std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()),bytes.size());
    if(!file) throw std::runtime_error("Could not export WAV");
}
}
int main(int argc,char** argv) {
    fs::path assets=argc>1?fs::u8path(argv[1]):fs::path("assets");
    fs::create_directories(assets/"images");fs::create_directories(assets/"audio");
    wav(assets/"audio/orchard-loop.wav",composeMusic());
    for(int i=0;i<static_cast<int>(Cue::Count);i++) wav(assets/cueFiles[i],composeEffect(static_cast<Cue>(i)));
    SetTraceLogLevel(LOG_WARNING);SetConfigFlags(FLAG_WINDOW_HIDDEN|FLAG_MSAA_4X_HINT);InitWindow(64,64,"Asset authoring");
    png(assets/"images/grass-board.png",1440,1080,[]{
        for(int y=0;y<18;y++) for(int x=0;x<24;x++) {
            Color color=(x+y)%2?Color{204,223,155,255}:Color{210,228,163,255};
            DrawRectangle(x*60,y*60,60,60,color);
            for(int dot=0;dot<10;dot++) {
                int n=x*229+y*911+dot*53;
                DrawCircle(x*60+(n*31)%58+1,y*60+(n*47)%58+1,1.1f,Color{180,205,136,40});
            }
            if((x*17+y*37)%29==0) {
                float px=x*60.f+22,py=y*60.f+40;
                DrawLineEx({px,py},{px-4,py-8},2,Color{161,191,119,125});
                DrawLineEx({px,py},{px+6,py-6},2,Color{161,191,119,125});
            }
        }
    });
    png(assets/"images/ui-card.png",192,192,[]{
        DrawRectangleRounded({1,7,190,184},.22f,20,Color{170,189,133,110});
        DrawRectangleRounded({1,1,190,184},.22f,20,Color{219,224,190,255});
        DrawRectangleRounded({2.5f,2.5f,187,181},.22f,20,WHITE);
    });
    png(assets/"images/ui-button.png",144,72,[]{
        DrawRectangleRounded({0,4,144,68},.4f,18,Color{204,214,176,255});
        DrawRectangleRounded({0,0,144,68},.4f,18,WHITE);
    });
    // A seamless material across the body, with no per-cell caps or circular borders.
    png(assets/"images/snake-skin.png",128,96,[]{
        struct Stop {int y;Color color;};
        const Stop stops[]={{0,{68,103,39,255}},{6,{68,103,39,255}},{11,{170,204,84,255}},
            {25,{204,224,114,255}},{48,{164,198,77,255}},{66,{163,193,80,255}},
            {75,{225,222,151,255}},{84,{220,214,142,255}},{90,{68,103,39,255}},{95,{68,103,39,255}}};
        for(int y=0;y<96;y++) {
            int index=0;while(index<8&&y>stops[index+1].y) index++;
            float t=(y-stops[index].y)/static_cast<float>(stops[index+1].y-stops[index].y);
            Color a=stops[index].color,b=stops[index+1].color;
            Color c{static_cast<unsigned char>(a.r+(b.r-a.r)*t),static_cast<unsigned char>(a.g+(b.g-a.g)*t),static_cast<unsigned char>(a.b+(b.b-a.b)*t),255};
            DrawRectangle(0,y,128,1,c);
        }
        DrawEllipse(38,42,9,6,Color{148,182,71,255});
        DrawEllipse(93,49,6,4,Color{152,186,74,255});
    });
    CloseWindow();std::cout<<"Exported six WAV files and four PNG textures into "<<assets.u8string()<<'\n';
}

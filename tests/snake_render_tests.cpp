#include "art.hpp"
#include <filesystem>
#include <iostream>
using namespace orchard;
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) return 2;
    SetTraceLogLevel(LOG_WARNING);SetConfigFlags(FLAG_WINDOW_HIDDEN);InitWindow(640,200,"Snake rendering regression");
    AssetFiles files{std::filesystem::path(argv[1])};
    if(!art::loadAssets(files)) {CloseWindow();return 2;}
    auto canvas=LoadRenderTexture(640,200);
    std::vector<Vector2> path;for(int i=0;i<17;i++) path.push_back({550.f-i*30,64});
    BeginTextureMode(canvas);ClearBackground(BLANK);art::snakeBody(path);EndTextureMode();
    Image image=LoadImageFromTexture(canvas.texture);ImageFlipVertical(&image);auto pixels=LoadImageColors(image);
    int gaps=0,wrongColor=0;
    for(int x=140;x<520;x++) {
        Color p=pixels[64*640+x];
        gaps+=p.a<240;
        wrongColor+=p.r<110||p.r>220||p.g<130||p.b>175;
    }
    UnloadImageColors(pixels);UnloadImage(image);UnloadRenderTexture(canvas);art::unloadAssets();CloseWindow();
    if(gaps||wrongColor) {std::cerr<<"FAIL: continuous green textured body; gaps="<<gaps<<", incorrect material pixels="<<wrongColor<<'\n';return 1;}
    std::cout<<"Long body rendered with continuous green PNG skin and no internal dark rings\n";
}

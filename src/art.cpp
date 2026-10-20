#include "art.hpp"
#include "snake_ribbon.hpp"
#include "rlgl.h"
#include <cmath>
#include <algorithm>
#include <vector>
namespace orchard::art {
namespace {
struct Images {Texture2D atlas{},mascot{},background{},board{},card{},button{},skin{};} images;
Texture2D texture(const AssetFiles& files,const char* path) {
    auto bytes=files.read(path);if(bytes.empty()) return {};
    Image decoded=LoadImageFromMemory(".png",bytes.data(),static_cast<int>(bytes.size()));
    if(!IsImageValid(decoded)) return {};
    Texture2D result=LoadTextureFromImage(decoded);UnloadImage(decoded);
    GenTextureMipmaps(&result);SetTextureFilter(result,TEXTURE_FILTER_TRILINEAR);return result;
}
void patch(Texture2D image,Rectangle r,Color tint,int margin=24) {
    NPatchInfo info{{0,0,static_cast<float>(image.width),static_cast<float>(image.height)},margin,margin,margin,margin,NPATCH_NINE_PATCH};
    DrawTextureNPatch(image,info,r,{0,0},0,tint);
}
}
bool loadAssets(const AssetFiles& files) {
    images.atlas=texture(files,"images/sprites.png");images.mascot=texture(files,"images/mascot.png");
    images.background=texture(files,"images/backdrop.png");images.board=texture(files,"images/grass-board.png");
    images.card=texture(files,"images/ui-card.png");images.button=texture(files,"images/ui-button.png");
    images.skin=texture(files,"images/snake-skin.png");
    SetTextureWrap(images.skin,TEXTURE_WRAP_REPEAT);
    for(auto t:{images.atlas,images.mascot,images.background,images.board,images.card,images.button,images.skin}) if(!IsTextureValid(t)) return false;
    return true;
}
void unloadAssets() {
    for(auto t:{images.atlas,images.mascot,images.background,images.board,images.card,images.button,images.skin}) if(IsTextureValid(t)) UnloadTexture(t);
    images={};
}
void sprite(Sprite value,Vector2 center,Vector2 size,float angle,Color tint) {
    int index=static_cast<int>(value);float w=images.atlas.width/4.f,h=images.atlas.height/4.f;
    Rectangle source{(index%4)*w+1,(index/4)*h+1,w-2,h-2};
    DrawTexturePro(images.atlas,source,{center.x,center.y,size.x,size.y},{size.x/2,size.y/2},angle,tint);
}
void backdropImage() {DrawTexturePro(images.background,{0,0,static_cast<float>(images.background.width),static_cast<float>(images.background.height)},{0,0,1280,800},{0,0},0,WHITE);}
void boardImage(Rectangle r) {DrawTexturePro(images.board,{0,0,static_cast<float>(images.board.width),static_cast<float>(images.board.height)},r,{0,0},0,WHITE);}
void snakeBody(const std::vector<Vector2>& points) {
    std::vector<RibbonPoint> path;path.reserve(points.size());
    for(auto p:points) path.push_back({p.x,p.y});
    for(const auto& strip:snakeRibbon(path)) {
        const auto& vertices=strip.vertices;std::size_t pairs=vertices.size()/2;
        for(std::size_t start=0;start+1<pairs;start+=256) {
            std::size_t end=std::min(pairs-1,start+256);
            rlCheckRenderBatchLimit(static_cast<int>((end-start)*6));
            // Changing from quads to triangles resets rlgl's texture binding.
            rlBegin(RL_TRIANGLES);rlSetTexture(images.skin.id);rlColor4ub(255,255,255,255);
            auto vertex=[](const RibbonVertex& v){rlTexCoord2f(v.u,v.v);rlVertex2f(v.x,v.y);};
            for(std::size_t i=start;i<end;i++) {
                vertex(vertices[i*2]);vertex(vertices[(i+1)*2]);vertex(vertices[i*2+1]);
                vertex(vertices[i*2+1]);vertex(vertices[(i+1)*2]);vertex(vertices[(i+1)*2+1]);
            }
            rlEnd();rlSetTexture(0);
        }
    }
}
void buttonImage(Rectangle r,Color tint) {patch(images.button,r,tint,22);}
void rounded(Rectangle r,float radius,Color color) {
    DrawRectangleRounded(r,std::min(1.f,2*radius/std::min(r.width,r.height)),12,color);
}
void panel(Rectangle r,Color fill) {
    patch(images.card,{r.x,r.y,r.width,r.height+6},fill,26);
}
void leaf(Vector2 p,float size,float angle,Color color) {
    (void)color;sprite(Sprite::Leaf,{p.x+std::cos(angle)*size*.5f,p.y+std::sin(angle)*size*.5f},{size,size*.75f},angle*RAD2DEG);
}
void flower(Vector2 p,float s,Color petals) {
    sprite(petals.g<220?Sprite::PinkFlower:Sprite::Daisy,p,{s*2.7f,s*2.7f});
}
void apple(Vector2 p,float s,float a) {
    sprite(Sprite::Apple,{p.x,p.y-s*.1f},{s*2.5f,s*2.5f},a*RAD2DEG);
}
void cloud(Vector2 p,float s,Color color) {
    (void)color;sprite(Sprite::Cloud,p,{s*1.8f,s*1.4f});
}
void star(Vector2 p,float radius,Color c,float angle) {
    (void)c;sprite(Sprite::Star,p,{radius*2.5f,radius*2.5f},angle*RAD2DEG);
}
void mascot(Vector2 p,float s,float t,bool sleepy) {
    float size=s*2.5f,bob=std::sin(t*1.8f)*s*.015f;
    DrawTexturePro(images.mascot,{0,0,static_cast<float>(images.mascot.width),static_cast<float>(images.mascot.height)},
                   {p.x-size/2,p.y-size*.43f+bob,size,size},{0,0},sleepy?-3.f:0.f,WHITE);
}
void hedge(Vector2 p,float s) {
    sprite(Sprite::Hedge,p,{s*1.18f,s*1.18f});
}
bool Type::load(const AssetFiles& files) {
    std::vector<int> chars; for(int c=32;c<384;c++) chars.push_back(c);
    chars.push_back(8212); chars.push_back(8211); chars.push_back(8226); chars.push_back(8592); chars.push_back(8594);
    auto load=[&](const char* name,int size){
        auto bytes=files.read(name);if(bytes.empty()) return Font{};
        Font f=LoadFontFromMemory(".ttf",bytes.data(),static_cast<int>(bytes.size()),size,chars.data(),static_cast<int>(chars.size()));
        SetTextureFilter(f.texture,TEXTURE_FILTER_BILINEAR); return f;
    };
    body=load("fonts/BalsamiqSans-Regular.ttf",48); bold=load("fonts/BalsamiqSans-Bold.ttf",64); title=load("fonts/BalsamiqSans-Bold.ttf",100);
    return IsFontValid(body)&&IsFontValid(bold)&&IsFontValid(title)
        &&body.texture.id!=GetFontDefault().texture.id&&bold.texture.id!=GetFontDefault().texture.id&&title.texture.id!=GetFontDefault().texture.id;
}
void Type::unload() {
    for(auto f:{body,bold,title}) if(IsFontValid(f)&&f.texture.id!=GetFontDefault().texture.id) UnloadFont(f);
}
void Type::text(const std::string& s,float x,float y,float size,Color c,bool heavy) const { DrawTextEx(heavy?bold:body,s.c_str(),{x,y},size,.2f,c); }
void Type::center(const std::string& s,float x,float y,float size,Color c,bool heavy) const { text(s,x-width(s,size,heavy)/2,y,size,c,heavy); }
void Type::heading(const std::string& s,float x,float y,float size,Color c) const { DrawTextEx(title,s.c_str(),{x,y},size,-1.f,c); }
float Type::width(const std::string& s,float size,bool heavy) const {return MeasureTextEx(heavy?bold:body,s.c_str(),size,.2f).x;}
}

#pragma once
#include "raylib.h"
#include "asset_files.hpp"
#include <string>
#include <vector>
namespace orchard::art {
inline constexpr Color ink{43,67,52,255}, muted{98,113,90,255}, paper{255,250,232,255};
inline constexpr Color cream{248,242,215,255}, green{66,128,75,255}, lime{153,204,85,255};
inline constexpr Color orange{242,151,67,255}, coral{225,92,73,255}, border{220,224,187,255};
enum class Sprite { Head, Blink, Munch, Dizzy, Body, Corner, Tail, Apple, Hedge, Daisy, PinkFlower, Leaf, Cloud, Sun, Star, Grass };
bool loadAssets(const AssetFiles& files);
void unloadAssets();
void sprite(Sprite value,Vector2 center,Vector2 size,float angle=0,Color tint=WHITE);
void backdropImage();
void boardImage(Rectangle area);
void snakeBody(const std::vector<Vector2>& points);
void buttonImage(Rectangle area,Color tint);
void rounded(Rectangle r,float radius,Color color);
void panel(Rectangle r,Color fill=paper);
void leaf(Vector2 p,float size,float angle,Color color=green);
void flower(Vector2 p,float size,Color petals);
void apple(Vector2 p,float size,float angle=0);
void cloud(Vector2 p,float size,Color color);
void star(Vector2 p,float radius,Color color,float angle=0);
void mascot(Vector2 p,float size,float time,bool sleepy=false);
void hedge(Vector2 p,float size);
struct Type {
    Font body{}, bold{}, title{};
    bool load(const AssetFiles& files);
    void unload();
    void text(const std::string& value,float x,float y,float size,Color color=ink,bool heavy=false) const;
    void center(const std::string& value,float x,float y,float size,Color color=ink,bool heavy=false) const;
    void heading(const std::string& value,float x,float y,float size,Color color=ink) const;
    float width(const std::string& value,float size,bool heavy=false) const;
};
}

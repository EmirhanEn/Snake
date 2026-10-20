#pragma once
#include <vector>
namespace orchard {
struct RibbonPoint { float x,y; };
struct RibbonVertex { float x,y,u,v; };
struct RibbonStrip { std::vector<RibbonVertex> vertices; };
// Points run from the head to the tail; large jumps separate wrapped sections.
std::vector<RibbonStrip> snakeRibbon(const std::vector<RibbonPoint>& points,float width=24.f,float breakDistance=45.f);
}

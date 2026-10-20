#include "snake_ribbon.hpp"
#include <algorithm>
#include <cmath>
namespace orchard {
namespace {
float distance(RibbonPoint a,RibbonPoint b) {return std::hypot(a.x-b.x,a.y-b.y);}
RibbonPoint lerp(RibbonPoint a,RibbonPoint b,float t) {return {a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t};}
void line(std::vector<RibbonPoint>& out,RibbonPoint end) {
    RibbonPoint start=out.back();int steps=std::max(1,static_cast<int>(std::ceil(distance(start,end)/2.5f)));
    for(int i=1;i<=steps;i++) out.push_back(lerp(start,end,i/static_cast<float>(steps)));
}
RibbonStrip makeStrip(const std::vector<RibbonPoint>& points,float width,bool tail,float uvOffset) {
    std::vector<RibbonPoint> smooth{points.front()};
    for(std::size_t i=1;i+1<points.size();i++) {
        const auto prev=points[i-1],p=points[i],next=points[i+1];
        float before=distance(prev,p),after=distance(p,next);
        float radius=std::min({8.f,before*.32f,after*.32f});
        auto enter=lerp(p,prev,radius/before),exit=lerp(p,next,radius/after);
        line(smooth,enter);
        for(int step=1;step<=10;step++) {
            float t=step/10.f;
            smooth.push_back(lerp(lerp(enter,p,t),lerp(p,exit,t),t));
        }
    }
    auto end=points.back();
    if(tail) {auto prev=points[points.size()-2];float len=distance(prev,end);end={end.x+(end.x-prev.x)*7/len,end.y+(end.y-prev.y)*7/len};}
    line(smooth,end);
    std::vector<float> arc(smooth.size(),0);
    for(std::size_t i=1;i<smooth.size();i++) arc[i]=arc[i-1]+distance(smooth[i-1],smooth[i]);
    RibbonStrip result;result.vertices.reserve(smooth.size()*2);
    for(std::size_t i=0;i<smooth.size();i++) {
        auto prev=smooth[i?i-1:i],next=smooth[i+1<smooth.size()?i+1:i];
        float dx=next.x-prev.x,dy=next.y-prev.y,len=std::hypot(dx,dy);
        if(len<.0001f) {dx=-1;dy=0;len=1;}
        float half=width*.5f;
        if(tail) {
            float t=std::clamp((arc.back()-arc[i])/38.f,0.f,1.f);
            half*=std::sin(t*1.57079632679f);
        }
        float nx=-dy/len*half,ny=dx/len*half,u=(arc[i]+uvOffset)/120.f;
        result.vertices.push_back({smooth[i].x+nx,smooth[i].y+ny,u,0});
        result.vertices.push_back({smooth[i].x-nx,smooth[i].y-ny,u,1});
    }
    return result;
}
}
std::vector<RibbonStrip> snakeRibbon(const std::vector<RibbonPoint>& points,float width,float breakDistance) {
    std::vector<RibbonStrip> result;
    if(points.size()<2||width<=0||breakDistance<=0) return result;
    std::vector<RibbonPoint> section;float offset=0;
    auto finish=[&](bool tail) {
        if(section.size()>1) {
            auto strip=makeStrip(section,width,tail,offset);
            offset=strip.vertices.back().u*120.f;
            result.push_back(std::move(strip));
        }
        section.clear();
    };
    for(auto p:points) {
        if(!section.empty()) {
            float jump=distance(section.back(),p);
            if(jump<.001f) continue;
            if(jump>breakDistance) finish(false);
        }
        section.push_back(p);
    }
    finish(true);return result;
}
}

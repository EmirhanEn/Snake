#include "snake_ribbon.hpp"
#include <cmath>
#include <iostream>
using namespace orchard;
int failures=0,checks=0;
void check(bool value,const char* label) {checks++;if(!value){failures++;std::cerr<<"FAIL: "<<label<<'\n';}}
float width(const RibbonVertex& a,const RibbonVertex& b) {return std::hypot(a.x-b.x,a.y-b.y);}
int main() {
    std::vector<RibbonPoint> straight;for(int i=0;i<14;i++) straight.push_back({450.f-i*30,100});
    auto longSnake=snakeRibbon(straight);
    check(longSnake.size()==1,"long straight snake is one continuous surface");
    if(!longSnake.empty()) {
        const auto& v=longSnake[0].vertices;
        check(v.size()>200 && v.size()%2==0,"surface has paired, closely spaced cross sections");
        bool constant=true,continuous=true;
        for(std::size_t i=0;i+1<v.size();i+=2) {
            if(v[i].u<2.7f && std::abs(width(v[i],v[i+1])-24)>0.02f) constant=false;
            if(i>0 && (v[i].u<=v[i-2].u || std::hypot(v[i].x-v[i-2].x,v[i].y-v[i-2].y)>4.5f)) continuous=false;
        }
        check(constant,"straight sections do not shrink into repeated round beads");
        check(continuous,"texture coordinates flow along the entire body without resetting at cells");
        check(v.size()>=2&&width(v[v.size()-2],v.back())<.1f,"tail finishes at a tapered point");
    }
    auto bent=snakeRibbon({{180,60},{150,60},{120,60},{120,90},{120,120},{90,120},{60,120}});
    check(bent.size()==1,"successive turns remain in the same strip");
    if(!bent.empty()) {
        bool safe=true;
        for(auto v:bent[0].vertices) if(!std::isfinite(v.x)||!std::isfinite(v.y)||v.x<47||v.x>193||v.y<47||v.y>133) safe=false;
        check(safe,"rounded corners have no spikes or invalid vertices");
    }
    auto wrap=snakeRibbon({{45,100},{15,100},{705,100},{675,100},{645,100}});
    check(wrap.size()==2,"wrapping never draws a stripe across the board");
    auto dup=snakeRibbon({{100,100},{100,100},{70,100},{40,100}});
    check(dup.size()==1,"duplicate growth positions do not break the mesh");
    check(snakeRibbon({}).empty()&&snakeRibbon({{1,1}}).empty(),"empty or single-point bodies are safe");
    std::cout<<checks<<" ribbon checks, "<<failures<<" failures\n";return failures?1:0;
}

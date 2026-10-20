#include "game.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace orchard;
int checks=0, failures=0;
void check(bool result, const char* message) {
    ++checks;
    if (!result) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
int main() {
    Game g(123);
    g.reset(Mode::Classic,Difficulty::Normal); g.food={9,9};
    check(g.step()==Event::Ate && g.snake.size()==4 && g.score==10,"fruit grows snake and awards ten points");
    g.reset(Mode::Classic,Difficulty::Normal);
    check(!g.turn(Direction::Left),"reject immediate reversal");
    check(g.turn(Direction::Up) && g.turn(Direction::Left),"buffer two deliberate turns");
    check(!g.turn(Direction::Down),"do not accept a third buffered turn");
    g.food={0,0}; g.step();
    check(g.snake[0]==Cell{8,8},"first buffered turn runs on first tick");
    g.step(); check(g.snake[0]==Cell{7,8},"second buffered turn runs on next tick");
    g.reset(Mode::Classic,Difficulty::Normal); g.turn(Direction::Up);
    check(!g.turn(Direction::Down),"reject reversal against queued direction");
    g.reset(Mode::Classic,Difficulty::Normal); g.snake={{23,9},{22,9},{21,9}}; g.food={0,0};
    check(g.step()==Event::Died && !g.alive,"classic wall collision kills");
    g.reset(Mode::Endless,Difficulty::Normal); g.snake={{23,9},{22,9},{21,9}}; g.food={0,0};
    check(g.step()==Event::Moved && g.snake[0]==Cell{0,9},"endless wraps right edge");
    g.reset(Mode::Endless,Difficulty::Normal); g.snake={{0,0},{0,1},{1,1}}; g.direction=Direction::Up; g.food={5,5};
    check(g.step()==Event::Moved && g.snake[0]==Cell{0,17},"endless wraps top edge");
    g.reset(Mode::Classic,Difficulty::Normal); g.snake={{2,2},{2,3},{3,3},{3,2},{3,1}}; g.food={0,0};
    check(g.step()==Event::Died,"body collision kills");
    g.reset(Mode::Classic,Difficulty::Normal); g.snake={{2,2},{2,3},{3,3},{3,2}}; g.food={0,0};
    check(g.step()==Event::Moved && g.snake.size()==4,"moving into a vacating tail is legal");
    g.reset(Mode::Challenge,Difficulty::Normal);
    check(!g.hedges.empty(),"challenge has hedges");
    g.snake={{4,4},{3,4},{2,4}}; g.hedges={{5,4}}; g.food={0,0};
    check(g.step()==Event::Died,"hedges collide");
    g.reset(Mode::Classic,Difficulty::Normal); g.snake.clear();
    for(int y=0;y<Game::rows;y++) for(int x=0;x<Game::cols;x++) if(x!=1 || y!=0) g.snake.push_back({x,y});
    g.food={1,0};
    check(g.step()==Event::Won && g.won && g.snake.size()==432,"full board wins without choosing an empty food list");
    g.reset(Mode::Challenge,Difficulty::Hard);
    for(int i=0;i<150;i++) {
        check(g.placeFood() && !g.blocked(g.food) && std::find(g.snake.begin(),g.snake.end(),g.food)==g.snake.end(),"food avoids snake and hedges");
    }
    g.eaten=10000; check(g.interval()>=.065f,"speed has a playable lower bound");
    g.reset(Mode::Classic,Difficulty::Easy); float easy=g.interval();
    g.reset(Mode::Classic,Difficulty::Hard); check(g.interval()<easy,"difficulty changes actual tick speed");
    g.turn(Direction::Up); g.reset(Mode::Classic,Difficulty::Normal); g.food={0,0}; g.step();
    check(g.snake[0]==Cell{9,9} && g.score==0 && g.alive,"restart clears turns and score");
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures ? 1 : 0;
}

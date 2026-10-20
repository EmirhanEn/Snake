#include "game.hpp"
#include <algorithm>
namespace orchard {
Cell delta(Direction d) {
    switch(d) {
        case Direction::Right: return {1,0};
        case Direction::Down: return {0,1};
        case Direction::Left: return {-1,0};
        default: return {0,-1};
    }
}
void Game::reset(Mode m, Difficulty d) {
    mode=m; difficulty=d; snake={{8,9},{7,9},{6,9}}; previous=snake;
    direction=Direction::Right; turns.clear(); hedges.clear();
    score=0; eaten=0; alive=true; won=false;
    if(mode==Mode::Challenge) {
        for(int x : {4,5,6,17,18,19}) for(int y : {4,13}) hedges.push_back({x,y});
        for(int y : {7,8,9,10}) for(int x : {3,20}) hedges.push_back({x,y});
    }
    placeFood();
}
bool Game::turn(Direction d) {
    const Direction last=turns.empty()?direction:turns.back();
    if(!alive || turns.size()>=2 || d==last || (static_cast<int>(d)+2)%4==static_cast<int>(last)) return false;
    turns.push_back(d); return true;
}
Event Game::step() {
    if(!alive) return won?Event::Won:Event::Died;
    if(!turns.empty()) { direction=turns.front(); turns.pop_front(); }
    Cell movement=delta(direction), next={snake.front().x+movement.x,snake.front().y+movement.y};
    if(mode==Mode::Endless) { next.x=(next.x+cols)%cols; next.y=(next.y+rows)%rows; }
    bool eating=next==food;
    auto end=eating?snake.end():snake.end()-1;
    if(next.x<0 || next.y<0 || next.x>=cols || next.y>=rows || blocked(next) || std::find(snake.begin(),end,next)!=end) {
        alive=false; return Event::Died;
    }
    previous=snake;
    snake.insert(snake.begin(),next);
    if(!eating) snake.pop_back();
    else {
        score+=10; eaten++;
        if(!placeFood()) { alive=false; won=true; return Event::Won; }
    }
    return eating?Event::Ate:Event::Moved;
}
bool Game::placeFood() {
    std::vector<Cell> free;
    for(int y=0;y<rows;y++) for(int x=0;x<cols;x++) {
        Cell c{x,y};
        if(!blocked(c) && std::find(snake.begin(),snake.end(),c)==snake.end()) free.push_back(c);
    }
    if(free.empty()) return false;
    food=free[std::uniform_int_distribution<std::size_t>(0,free.size()-1)(random)];
    return true;
}
float Game::interval() const {
    float base=difficulty==Difficulty::Easy?.175f:difficulty==Difficulty::Normal?.135f:.105f;
    return std::max(.068f,base-eaten*.0018f);
}
bool Game::blocked(Cell p) const { return std::find(hedges.begin(),hedges.end(),p)!=hedges.end(); }
}

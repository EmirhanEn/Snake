#pragma once
#include <deque>
#include <random>
#include <vector>

namespace orchard {
struct Cell { int x{}, y{}; bool operator==(Cell p) const { return x == p.x && y == p.y; } };
enum class Direction { Right, Down, Left, Up };
enum class Mode { Classic, Endless, Challenge };
enum class Difficulty { Easy, Normal, Hard };
enum class Event { Moved, Ate, Died, Won };
Cell delta(Direction d);

class Game {
public:
    static constexpr int cols = 24, rows = 18;
    explicit Game(unsigned seed = 42) : random(seed) {}
    void reset(Mode m, Difficulty d);
    bool turn(Direction d);
    Event step();
    bool placeFood();
    float interval() const;
    bool blocked(Cell p) const;
    std::vector<Cell> snake;
    std::vector<Cell> previous;
    std::vector<Cell> hedges;
    Cell food{};
    Direction direction = Direction::Right;
    Mode mode = Mode::Classic;
    Difficulty difficulty = Difficulty::Normal;
    bool alive = true, won = false;
    int score = 0, eaten = 0;
private:
    std::mt19937 random;
    std::deque<Direction> turns;
};
}

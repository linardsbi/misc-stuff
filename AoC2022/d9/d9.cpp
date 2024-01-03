#include <fstream>
#include <iostream>
#include <sstream>
#include <ranges>
#include <string_view>
#include <vector>
#include <numeric>

using namespace std::literals;

constexpr std::size_t GRID_SIZE = 1000;
static std::array<bool, GRID_SIZE * GRID_SIZE> grid{};

struct vec2 {
    int x{};
    int y{};

    void operator+=(const vec2 other) {
        this->x += other.x;
        this->y += other.y;
    }
    vec2 operator-(const vec2 other) const {
        return {this->x - other.x, this->y - other.y};
    }

    int distance(const vec2 other) const {
        const auto difference = *this - other;
        return std::max(std::abs(difference.x), std::abs(difference.y));
    }
};


int main() {
    auto file = std::ifstream("input.txt");

    auto head = vec2{500, 500};
    auto tail = head;
    vec2 last_head_pos{};

    auto move_tail = [&tail, &head, &last_head_pos]() {
        if (head.distance(tail) > 1) {
            tail = last_head_pos;
            const auto offset = static_cast<std::size_t>(tail.y) * GRID_SIZE +
                                static_cast<std::size_t>(tail.x);
            grid[offset] = true;
        }
    };

    // if going right, add speed {1, 0}
    // if going bottom, add speed {0, -1}
    // if going left, add speed {-1, 0}
    // if going up, add speed {0, 1}

    constexpr auto RIGHT = vec2{1, 0};
    constexpr auto DOWN = vec2{0, -1};
    constexpr auto LEFT = vec2{-1, 0};
    constexpr auto UP = vec2{0, 1};

    std::stringstream line;
    line << file.rdbuf();

    while (!line.eof()) {
        char direction{};
        uint32_t steps{};

        line >> direction;
        line >> steps;

        while (steps-- > 0) {
            vec2 new_direction;
            switch (direction) {
                case 'R':
                    new_direction = RIGHT;
                    break;
                case 'D':
                    new_direction = DOWN;
                    break;
                case 'L':
                    new_direction = LEFT;
                    break;
                case 'U':
                    new_direction = UP;
                    break;
                default:
                    std::cout << "Unreachable\n";
            }
            last_head_pos = head;
            head += new_direction;
            move_tail();
        }
    }

    std::cout << "Head position " << head.x << ' ' << head.y << '\n';
    std::cout << "Tail position " << tail.x << ' ' << tail.y << '\n';

    const auto total_visits = std::accumulate(grid.begin(), grid.end(), 1);

    std::cout << "Tail visited " << total_visits << " squares\n";
}


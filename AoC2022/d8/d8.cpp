#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <ranges>
#include <string_view>
#include <vector>

using namespace std::literals;

enum class Visibility : uint8_t {
    Left = 0b0001,
    Top = 0b0010,
    Right = 0b0100,
    Bottom = 0b1000
};
static std::array<Visibility, 10'000> trees{};

void set_visible_from(const auto coordinate, const Visibility direction) {
    trees[coordinate] = static_cast<Visibility>(static_cast<uint8_t>(trees[coordinate]) | static_cast<uint8_t>(direction));
}

// this is what you get when you try to create a smarter solution before doing the obvious one
int main() {
    auto file = std::ifstream("input.txt");
    uint64_t total_visible = 0;
    std::stringstream stream;
    stream << file.rdbuf();
    std::string_view lines = stream.view();

    std::cout << "char count " << lines.length() << '\n';

    std::size_t line_num{};
    std::size_t line_len{};
    auto split_view = std::ranges::split_view(lines, "\n"sv);
    std::array<uint8_t, 150> largest_y{};
    for (std::size_t nl_offset{}; const auto line_ptr : split_view) {
        std::string_view line{line_ptr};
        const auto y_coord = line_num * line.length() + nl_offset;

        line_len = line.length();

        uint8_t largest_x = 0;
        for (std::size_t char_n{}; const auto this_height : line) {

            if (this_height > largest_x) {
                set_visible_from(y_coord + char_n, Visibility::Left);
            }
            if (this_height > largest_y[char_n]) {
                set_visible_from(y_coord + char_n, Visibility::Top);
            }

            if (this_height > largest_x) {
                largest_x = static_cast<uint8_t>(this_height);
            }
            if (this_height > largest_y[char_n]) {
                largest_y[char_n] = static_cast<uint8_t>(this_height);
            }

            ++char_n;
        }
        ++line_num;
        ++nl_offset;
    }

    largest_y = {};

    for (auto current = line_num - 1, nl_offset = current; current != 0; --current, --nl_offset) {
        const auto line = lines.substr(current * line_len + nl_offset, line_len);
        const auto y_coord = current * line_len + nl_offset;
        uint8_t largest_x = 0;
        for (auto i = line_len - 1; i != 0; --i) {
            const auto this_height = line[i];

            if (this_height > largest_x) {
                set_visible_from(y_coord + i, Visibility::Right);
            }
            if (this_height > largest_y[i]) {
                set_visible_from(y_coord + i, Visibility::Bottom);
            }

            if (this_height > largest_x) {
                largest_x = static_cast<uint8_t>(this_height);
            }
            if (this_height > largest_y[i]) {
                largest_y[i] = static_cast<uint8_t>(this_height);
            }
        }
    }

    for (const auto tree : trees ) {
        if (static_cast<uint8_t>(tree) != 0) {
            ++total_visible;
        }
    }

    std::cout << "Visible count: " << total_visible << '\n';
}


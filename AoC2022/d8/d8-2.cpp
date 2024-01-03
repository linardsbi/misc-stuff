#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <ranges>
#include <string_view>
#include <vector>

using namespace std::literals;

uint32_t add_right_and_left(auto file_iter) {
    uint32_t total_right = 1;
    const auto start_iter = file_iter;
    while (*std::next(file_iter) && *std::next(file_iter) != '\n' && *start_iter > *std::next(file_iter)) {
        ++total_right;
        ++file_iter;
    }

    if (!*std::next(file_iter) || *std::next(file_iter) == '\n') {
        --total_right;
    }

    file_iter = start_iter;
    uint32_t total_left = 1;
    while (*std::prev(file_iter) && *std::prev(file_iter) != '\n' && *start_iter > *std::prev(file_iter)) {
        ++total_left;
        --file_iter;
    }

    if (!*std::prev(file_iter) || *std::prev(file_iter) == '\n') {
        --total_left;
    }

    return total_right * total_left;
}

uint32_t add_up_and_down(auto file_iter, const auto offset_distance, const auto lower_bound, const auto upper_bound) {
    const auto start_iter = file_iter;
    uint32_t total_down = 1;
    std::advance(file_iter, offset_distance);
    while (*file_iter && file_iter < upper_bound && *start_iter > *file_iter) {
        ++total_down;
        std::advance(file_iter, offset_distance);
    }
    if (file_iter >= upper_bound) {
        --total_down;
    }

    uint32_t total_up = 1;
    file_iter = start_iter;
    std::advance(file_iter, -offset_distance);

    while (*file_iter && file_iter >= lower_bound && *start_iter > *file_iter) {
        ++total_up;
        std::advance(file_iter, -offset_distance);
    }
    if (file_iter < lower_bound) {
        --total_up;
    }

    return total_down * total_up;
}

int main() {
    auto file = std::ifstream("input.txt");
    uint64_t high_score = 0;

    std::stringstream stream;
    stream << file.rdbuf();
    std::string_view lines = stream.view();

    for (std::size_t line_num{}; const auto line : std::ranges::split_view(lines, "\n"sv)) {
        if (line_num == 0 || line_num == 98) {
            ++line_num;
            continue;
        }
        for (std::size_t i = 0; i < line.size(); ++i) {
            if (i % (line.size() - 1) == 0) {
                continue;
            }
            const auto char_iter = lines.begin() + (line_num * line.size() + i + line_num);
            uint32_t distance_sum = add_right_and_left(char_iter);
            distance_sum *= add_up_and_down(char_iter, line.size() + 1, lines.begin(), lines.end());
            if (distance_sum > high_score) {
                high_score = distance_sum;
            }
        }
        ++line_num;
    }

    std::cout << "Highest scenic score: " << high_score << '\n';
}


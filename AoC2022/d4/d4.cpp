#include <fstream>
#include <iostream>
#include <array>
#include <sstream>
#include <ranges>

uint32_t has_fully_contained(const auto& section_assignments) {
    // contains if
    // 1_start >= 2_start && 1_end <= 2_end
    // or
    // 2_start >= 1_start && 2_end <= 1_end
    if (section_assignments.first.first >= section_assignments.second.first
     && section_assignments.first.second <= section_assignments.second.second) {
        return 1;
    }

    if (section_assignments.second.first >= section_assignments.first.first
        && section_assignments.second.second <= section_assignments.first.second) {
        return 1;
    }

    // overlaps if
    // 2_end >= 1_start && 2_start <= 1_start
    // or
    // 1_end >= 2_start && 2_start >= 1_start
    if (section_assignments.second.second >= section_assignments.first.first
        && section_assignments.second.first <= section_assignments.first.first) {
        return 1;
    }

    if (section_assignments.first.second >= section_assignments.second.first
        && section_assignments.second.first >= section_assignments.first.first) {
        return 1;
    }

    return 0;
}

int main() {
    using split = std::ranges::split_view<std::string_view, std::string_view>;

    auto file = std::ifstream("input.txt");
    uint32_t total_score{};
    std::stringstream stream;
    stream << file.rdbuf();

    for (; !stream.eof();) {
        std::string line;
        stream >> line;

        if (line.empty()) {
            break;
        }

        bool first = true;
        std::pair<std::pair<int, int>, std::pair<int, int>> section_assignments;

        for (const auto range: split(std::string_view{line}, std::string_view{","})) {
            auto numbers_subrange = split(std::string_view{range}, std::string_view{"-"});
            const auto first_num = numbers_subrange.begin();
            const auto second_num = std::next(first_num);

            auto range_to_int = [](const auto it) {
                return std::stoi((*it).begin());
            };

            if (first) {
                section_assignments.first.first = range_to_int(first_num);
                section_assignments.first.second = range_to_int(second_num);
            } else {
                section_assignments.second.first = range_to_int(first_num);
                section_assignments.second.second = range_to_int(second_num);
            }

            first = false;
        }

        total_score += has_fully_contained(section_assignments);
        section_assignments = {};
    }

    std::cout << "Total: " << total_score << "\n";
}


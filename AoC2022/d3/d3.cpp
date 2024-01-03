#include <fstream>
#include <iostream>
#include <array>
#include <sstream>
#include <ranges>

static constexpr auto letter_count = ('z' - 'a') * 2 + 1;
static std::array<uint32_t, letter_count> used_letters{};
static std::array<bool, letter_count> current_line_letters{};

auto& current_line_letter(const auto letter) {
    return current_line_letters[isupper(letter) ? static_cast<unsigned long>(letter - 'A' + 'z' - 'a')
                                        : static_cast<unsigned long>(
                                letter - 'a')];
}

auto& get_letter_use_count(const auto letter) {
    return used_letters[isupper(letter) ? static_cast<unsigned long>(letter - 'A' + 'z' - 'a')
                                        : static_cast<unsigned long>(
                                letter - 'a')];
}

void inc_letter_use_count(const auto letter) {
    auto& use_count = get_letter_use_count(letter);

    if (use_count > 0) {
        return;
    }

    ++use_count;
}

void inc_line_letter_use_count(const auto letter) {
    auto& use_count = get_letter_use_count(letter);
    auto& letter_in_current_line = current_line_letter(letter);

    if (use_count > 0 && letter_in_current_line) {
        return;
    }
    letter_in_current_line = true;
    ++use_count;
}

//int main() {
//    auto file = std::ifstream("input.txt");
//    uint32_t total_score{};
//    std::stringstream stream;
//    stream << file.rdbuf();
//
//    for (; !stream.eof();) {
//        std::string line;
//        stream >> line;
//
//        if (line.empty()) {
//            break;
//        }
//
//        for (std::size_t i = 0; i < line.size() / 2; ++i) {
//            inc_letter_use_count(line[i]);
//        }
//        for (std::size_t i = line.size() / 2; i < line.size(); ++i) {
//            const auto times_used = get_letter_use_count(line[i]);
//            if (times_used > 0) {
//                const auto letter_score = isupper(line[i]) ? static_cast<unsigned int>(line[i] - 'A' + 'z' - 'a' + 1)
//                                                           : static_cast<unsigned int>(
//                                                  line[i] - 'a');
//                total_score += letter_score;
//                ++total_score;
//                break;
//            }
//        }
//        used_letters = {};
//    }
//
//    std::cout << "Total: " << total_score << "\n";
//}

int main() {
    auto file = std::ifstream("input.txt");
    uint32_t total_score{};
    std::stringstream stream;
    stream << file.rdbuf();

    for (; !stream.eof();) {
        for (int i = 0; i < 3; ++i) {
            std::string line;
            stream >> line;

            if (line.empty()) {
                break;
            }
            for (const char j : line) {
                inc_line_letter_use_count(j);
            }
            current_line_letters = {};
        }
        for (std::size_t i = 0; i < letter_count; ++i) {
            if (used_letters[i] == 3) {
                total_score += static_cast<uint32_t>(i) + (i > 25 ? 2 : 1);
            }
        }

        used_letters = {};
    }

    std::cout << "Total: " << total_score << "\n";
}

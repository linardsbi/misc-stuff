#include <fstream>
#include <iostream>
#include <array>
#include <sstream>

bool no_chars_repeat(auto sequence) {
    constexpr auto alphabet_letter_count = 'z' - 'a' + 1;
    std::array<uint8_t, alphabet_letter_count> letters{};

    for (const auto letter: sequence) {
        const auto index = static_cast<std::size_t>(letter - 'a');
        if (++letters[index] > 1) {
            return false;
        }
    }

    return true;
}

int main() {
    auto file = std::ifstream("input.txt");
    std::stringstream stream;
    stream << file.rdbuf();

    // the start of a packet is indicated by a sequence of four characters that are all different
    constexpr auto sequence_len = 14;

    auto message = stream.view();
    std::size_t start_index = std::string::npos;

    for (std::size_t i = 0; i < message.size(); ++i) {
        if (no_chars_repeat(message.substr(i, sequence_len))) {
            start_index = i + sequence_len;
            break;
        }
    }

    if (start_index == std::string::npos) {
        std::cout << "Not found" << "\n";
    } else {
        std::cout << start_index << "\n";
    }
}


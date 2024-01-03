#include <array>
#include <fstream>
#include <iostream>
#include <deque>
#include <sstream>
#include <ranges>
#include <algorithm>

// parser:
// 1. get line,
// 2. read 3 chars, if not new line, increment stack count, else goto 1.
// 3. if first char is not space, push letter onto vector at stack count index
// 4. goto 2

static std::array<std::deque<char>, 9> stacks;

int main() {
    auto file = std::ifstream("input.txt");
    std::stringstream stream;
    stream << file.rdbuf();

    bool parse_stacks = true;
    for (std::string line; !stream.eof();) {
        std::getline(stream, line);

        if (line.empty()) {
            parse_stacks = false;
            continue;
        }

        std::string_view line_view = line;

        if (parse_stacks) {
            for (std::size_t span_start = 0, stack_id = 0; span_start < line.length(); ++stack_id) {
                auto crate = line_view.substr(span_start, span_start + 3);

                if (!crate.starts_with(' ')) {
                    stacks[stack_id].push_back(crate[1]);
                }

                span_start += 4;
            }
            continue;
        }

        std::stringstream instructions{line};
        std::string unused;
        uint32_t movable_count = 0;
        uint32_t move_from = 0;
        uint32_t move_to = 0;

        instructions >> unused;
        instructions >> movable_count;
        instructions >> unused;
        instructions >> move_from;
        instructions >> unused;
        instructions >> move_to;

        auto& from_stack = stacks[move_from - 1];
        auto& to_stack = stacks[move_to - 1];
        std::reverse_copy(from_stack.begin(), from_stack.begin() + movable_count, std::front_inserter(to_stack));
        from_stack.erase(from_stack.begin(), from_stack.begin() + movable_count);

//        for (uint32_t i = 0; i < movable_count; ++i) {
//            auto& stack = stacks[move_from - 1];
//            stacks[move_to - 1].push_front(stack.front());
//            stack.pop_front();
//        }
    }
    for (const auto& stack : stacks) {
        if (!stack.empty()) {
            std::cout << stack.front();
        }
    }
    std::cout << "\n";
}


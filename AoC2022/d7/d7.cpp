#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <ranges>
#include <string_view>
#include <vector>

using namespace std::literals;

struct Node {
    Node(std::string_view t_name, uint64_t t_size)
            : name(t_name), size(t_size) {}

    std::string name;
    std::vector<std::shared_ptr<Node>> children;
    std::shared_ptr<Node> parent;
    uint64_t size{};
};

static std::shared_ptr<Node> root_dir = std::make_shared<Node>("/", 0);
static std::shared_ptr<Node> current_dir;

void parse_command(std::string_view arguments) {
    auto command = arguments.substr(0, 2);
    if (command == "cd"sv) {
        const auto dir_name = arguments.substr(3);

        if (dir_name == "/"sv) {
            current_dir = root_dir;
            return;
        }

        if (dir_name == ".."sv) {
            current_dir = current_dir->parent;
            return;
        }

        const auto child = std::ranges::find_if(current_dir->children, [dir_name](const auto &item) {
            return item->name == dir_name;
        });

        if (child != current_dir->children.end()) {
            current_dir = *child;
        } else {
            std::cout << "Directory " << dir_name << " not found\n";
        }
    }
}

void add_dir(std::string_view name) {
    const auto new_dir = std::make_shared<Node>(name, 0);
    new_dir->parent = current_dir;
    current_dir->children.emplace_back(new_dir);
}

void add_file(const auto name, const auto size) {
    current_dir->children.emplace_back(std::make_shared<Node>(name, size));
}

void parse_file(std::string_view description) {
    const auto [size, name] = [description]() {
        std::array args{""sv, ""sv};
        for (std::size_t i = 0; const auto arg : std::ranges::split_view(description, " "sv)) {
            args[i++] = std::string_view{arg};
        }
        return args;
    }();

    if (size == "dir"sv) {
        add_dir(name);
        return;
    }

    add_file(name, std::stoul(size.data()));
}

void parse(std::string_view line) {
    if (line.starts_with('$')) {
        parse_command(line.substr(2));
        return;
    }

    parse_file(line);
}

//static uint64_t size_sum = 0;
static uint64_t smallest_to_free = static_cast<uint64_t>(-1);
static uint64_t to_free = 0;

uint64_t calc_size(const auto &dir) {
    uint64_t total = 0;
    for (const auto &item : dir->children) {
        if (item->size == 0) {
            total += calc_size(item);
        } else {
            total += item->size;
        }
    }

    std::cout << dir->name << " has size " << total << '\n';

//    if (total <= 100000) {
//        size_sum += total;
//    }

    if (to_free > 0 && to_free <= total && total < smallest_to_free) {
        smallest_to_free = total;
    }

    return total;
}

int main() {
    auto file = std::ifstream("input.txt");

    for (std::string line; !file.eof();) {
        std::getline(file, line);

        if (line.empty()) {
            break;
        }

        parse(line);
    }

    constexpr uint64_t filesystem_size = 70'000'000;
    constexpr uint64_t required_space = 30'000'000;

    const auto space_with_update = required_space + calc_size(root_dir);

    to_free = space_with_update - filesystem_size;

    // can walking the directory tree twice be avoided somehow?
    calc_size(root_dir);

//    std::cout << "The sum of the total sizes of directories <= 100k " << size_sum << '\n';
    std::cout << "Could free a folder of size " << smallest_to_free << '\n';
}


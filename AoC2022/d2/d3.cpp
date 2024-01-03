#include <fstream>
#include <iostream>
#include <array>
#include <sstream>
//0 if you lost, 3 if the round was a draw, and 6 if you won
//X means you need to lose, Y means you need to end the round in a draw, and Z means you need to win.
//X for Rock, Y for Paper, and Z for Scissors
static constexpr std::array outcomes = {
        std::array{'Z', 'X', 'Y'},
        std::array{'X', 'Y', 'Z'},
        std::array{'Y', 'Z', 'X'}
};
int main() {
	auto file = std::ifstream("input.txt");
    uint32_t total_score{};
    std::stringstream line;
    line << file.rdbuf();

	for (; !line.eof(); ) {

	}

	std::cout << "Total: " << total_score << "\n";
}

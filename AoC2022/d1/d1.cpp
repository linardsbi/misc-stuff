#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>
struct Elf {
	uint32_t calories = 0;
};

struct Dqueue {
	void add(uint32_t calories) {
		if (m_size == 0) {
			m_elfs.push_back({calories});
			m_size++;
			return;
		}

        bool inserted = false;
		for (std::size_t i = 0; i < m_size; ++i) {
			if (calories > m_elfs[i].calories) {
				m_elfs.insert(m_elfs.begin() + i, {calories});
                inserted = true;
                break;
			}
		}
        if (!inserted) {
            m_elfs.push_back({calories});
        }

		m_size++;
	}

	std::size_t m_size{};
	std::vector<Elf> m_elfs;
};

int main() {
	auto file = std::ifstream("input.txt");
	Elf current;
	Dqueue elves;

	for (std::string line; std::getline(file, line); ) {
		if (line.empty()) {
			elves.add(current.calories);
			current.calories = 0;
			continue;
		}

		current.calories += static_cast<unsigned int>(std::stoi(line));
	}

    elves.add(current.calories);
    if (elves.m_size == 0) {
        std::cout << "Nothing to do...\n";
    }
	const auto result = std::accumulate(elves.m_elfs.begin(), elves.m_elfs.begin() + 3, 0, [](int total, Elf elf) {return elf.calories + total;});
	std::cout << "Top 3 total: " << result << " calories.\n";
}

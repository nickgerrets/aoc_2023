#include "common.h"

#include <vector>

std::vector<std::string> parse_schematic(std::istream& input) {
	std::vector<std::string> schematic;
	for (auto& line : aoc::Lines(input)) {
		schematic.emplace_back(line);
	}
	return schematic;
}

bool is_symbol(char c) {
	return (!std::isdigit(c) && c != '.');
}

bool is_adjacent(std::vector<std::string> const& schematic, uint64_t x, uint64_t y) {
	bool adjacent = false;

	// can we check the left side?
	if (x > 0) {
		adjacent |= (y > 0 && is_symbol(schematic[y - 1][x - 1]));
		adjacent |= (is_symbol(schematic[y][x - 1]));
		adjacent |= (y < schematic.size() - 1 && is_symbol(schematic[y + 1][x - 1]));
	}

	// middle
	adjacent |= (y > 0 && is_symbol(schematic[y - 1][x]));
	adjacent |= (y < schematic.size() - 1 && is_symbol(schematic[y + 1][x]));

	// right
	if (x < schematic[0].length() - 1) {
		adjacent |= (y > 0 && is_symbol(schematic[y - 1][x + 1]));
		adjacent |= (is_symbol(schematic[y][x + 1]));
		adjacent |= (y < schematic.size() - 1 && is_symbol(schematic[y + 1][x + 1]));
	}

	return adjacent;
}

uint64_t parse_n(std::string const& line, uint64_t x) {
	uint64_t n = 0;
	while (std::isdigit(line[x])) {
		--x;
	}
	++x;
	while (std::isdigit(line[x])) {
		n = (n * 10) + line[x] - '0';
		++x;
	}
	return n;
}

uint64_t calculate_ratio(std::vector<std::string> const& schematic, uint64_t x, uint64_t y) {
	uint64_t ratio = 1;
	uint64_t count = 0;
	size_t yy = y;
	if (y > 0) {
		yy = y - 1;
	}
	for (yy; yy < schematic.size() && yy <= y + 1; ++yy) {
		size_t xx = x;
		if (x > 0) {
			xx = x - 1;
		}
		for (xx; xx < schematic[0].length() && xx <= x + 1; ++xx) {
			std::cout << "checking " << schematic[yy][xx] << std::endl;


			if (std::isdigit(schematic[yy][xx])) {
				uint64_t n = parse_n(schematic[yy], xx);

				std::cout << "Found " << n << " ratio_prev: " << ratio << std::endl;

				ratio *= n;
				++count;
				// skip digits that have already been parsed
				while (std::isdigit(schematic[yy][xx])) {
					++xx;
				}
			}
		}
	}

	if (count <= 1) {
		return 0;
	}

	return ratio;
}

uint64_t sum_parts(std::vector<std::string> const& schematic) {
	uint64_t sum = 0;

	for (size_t y = 0; y < schematic.size(); ++y) {
		for (size_t x = 0; x < schematic[0].length(); ++x) {
			if (std::isdigit(schematic[y][x])) {
				// parse and check number
				bool adjacent = false;
				uint64_t n = 0;
				while (std::isdigit(schematic[y][x]) && schematic[0].length()) {
					adjacent |= is_adjacent(schematic, x, y);
					n = (n * 10) + (schematic[y][x] - '0');
					++x;
				}
				if (adjacent) {
					sum += n;
				}
			}
		}
	}

	return sum;
}

uint64_t sum_gears(std::vector<std::string> const& schematic) {
	uint64_t sum = 0;
	for (size_t y = 0; y < schematic.size(); ++y) {
		for (size_t x = 0; x < schematic[0].length(); ++x) {
			if (schematic[y][x] == '*') {
				uint64_t ratio = calculate_ratio(schematic, x, y);

				std::cout << "Gear ratio: " << ratio << std::endl;

				sum += ratio;
			}
		}
	}
	return sum;
}



int main(int argc, char** argv) {
	auto file = aoc::get_input_file(argc, argv);

	std::vector<std::string> schematic = parse_schematic(file);

	// for (auto& line : schematic) {
	// 	std::cout << line << std::endl;
	// }

	std::cout << "Sum of all parts is: " << sum_parts(schematic) << std::endl;
	std::cout << "Sum of all gear ratios is: " << sum_gears(schematic) << std::endl;

	return (EXIT_SUCCESS);
}


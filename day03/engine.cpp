#include "common.h"

#include <vector>
#include <algorithm>

bool is_symbol(char c) {
	return (!std::isdigit(c) && c != '.');
}

bool is_adjacent(std::vector<std::string> const& schematic, size_t x, size_t y) {
	bool adjacent = false;

	size_t x_max = std::min(x + 2, (size_t)schematic[0].length());
	size_t y_max = std::min(y + 2, (size_t)schematic.size());
	size_t x_start = (x > 0) ? x - 1 : x;
	size_t y_start = (y > 0) ? y - 1 : y;

	for (y = y_start; y < y_max; ++y) {
		for (x = x_start; x < x_max; ++x) {
			adjacent |= is_symbol(schematic[y][x]);
		}
	}

	return adjacent;
}

// Part 1
uint64_t sum_parts(std::vector<std::string> const& schematic) {
	uint64_t sum = 0;
	for (size_t y = 0; y < schematic.size(); ++y) {
		for (size_t x = 0; x < schematic[0].length(); ++x) {
			if (std::isdigit(schematic[y][x])) {
				// parse and check number
				bool adjacent = false;
				uint64_t n = 0;
				// A simple unsigned parse without overflow check, combined with adjacent check
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

uint64_t parse_n(std::string const& line, size_t x) {
	uint64_t n = 0;
	// Go all the way to the start of the number
	while (x > 0 && std::isdigit(line[x - 1])) {
		--x;
	}
	// simple unsigned integer parse (without overflow check)
	while (std::isdigit(line[x]) && x < line.length()) {
		n = (n * 10) + line[x] - '0';
		++x;
	}
	return n;
}

uint64_t calculate_ratio(std::vector<std::string> const& schematic, size_t x, size_t y) {
	uint64_t ratio = 1;
	uint64_t count = 0;
	
	size_t x_max = std::min(x + 2, (size_t)schematic[0].length());
	size_t y_max = std::min(y + 2, (size_t)schematic.size());
	size_t x_start = (x > 0) ? x - 1 : x;
	size_t y_start = (y > 0) ? y - 1 : y;

	for (y = y_start; y < y_max; ++y) {
		for (x = x_start; x < x_max; ++x) {
			if (std::isdigit(schematic[y][x])) {
				// If we found a number, parse and multiply
				ratio *= parse_n(schematic[y], x);
				++count;

				// skip digits that have already been parsed
				while (std::isdigit(schematic[y][x])) {
					++x;
				}
			}
		}
	}

	// Just in place so gears with only 1 (or 0) adjacent numbers aren't counted
	if (count <= 1) {
		return 0;
	}
	return ratio;
}

// Part 2
uint64_t sum_gears(std::vector<std::string> const& schematic) {
	uint64_t sum = 0;
	for (size_t y = 0; y < schematic.size(); ++y) {
		for (size_t x = 0; x < schematic[0].length(); ++x) {
			if (schematic[y][x] == '*') {
				uint64_t ratio = calculate_ratio(schematic, x, y);
				sum += ratio;
			}
		}
	}
	return sum;
}

std::vector<std::string> parse_schematic(std::istream& input) {
	std::vector<std::string> schematic;
	for (auto& line : aoc::Lines(input)) {
		schematic.emplace_back(std::move(line));

		// Bad error handling
		if (line.length() != schematic[0].length()) {
			throw std::runtime_error("bad schematic");
		}
	}
	return schematic;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	// The schematic is basically a 2d-array
	std::vector<std::string> schematic = parse_schematic(*input);

	std::cout << "(part 1) Sum of all parts is:       " << sum_parts(schematic) << std::endl;
	std::cout << "(part 2) Sum of all gear ratios is: " << sum_gears(schematic) << std::endl;

	return (EXIT_SUCCESS);
}


#include "common.h"

#include <vector>
#include <sstream>

using pattern_t = std::vector<std::string>;
std::vector<pattern_t> parse_patterns(std::istream& stream) {
	std::vector<pattern_t> patterns;
	pattern_t p;
	for (auto const& line : aoc::Lines(stream)) {

		if (line.length() == 0) {
			patterns.push_back(p);
			p = pattern_t();
			continue;
		}

		p.push_back(line);
	}
	patterns.push_back(p);
	return patterns;
}

int64_t horizontal_difference(pattern_t const& p, size_t col) {
	int64_t d = std::min(col, p[0].length() - col);
	int64_t sum = 0;
	for (size_t y = 0; y < p.size(); ++y) {
		for (size_t x = 0; x < d; ++x) {
			if (p[y][col + x] != p[y][col - x - 1]) {
				++sum;
			}
		}
	}
	return sum;
}

int64_t vertical_difference(pattern_t const& p, size_t row) {
	int64_t d = std::min(row, p.size() - row);
	int64_t sum = 0;
	for (size_t y = 0; y < d; ++y) {
		for (size_t x = 0; x < p[0].length(); ++x) {
			if (p[row + y][x] != p[row - y - 1][x]) {
				++sum;
			}
		}
	}
	return sum;
}

template <int64_t MAX_DIFF = 0>
size_t pattern_reflection(pattern_t const& p) {
	static size_t const VERTICAL_MOD = 100;
	size_t reflection = 0;
	for (size_t col = 0; col < p[0].length(); ++col) {
		if (horizontal_difference(p, col) == MAX_DIFF) {
			reflection += col;
		}
	}

	for (size_t row = 0; row < p.size(); ++row) {
		if (vertical_difference(p, row) == MAX_DIFF) {
			reflection += row * VERTICAL_MOD;
		}
	}
	return reflection;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto patterns = parse_patterns(*input);

	std::cout << "(Part 1) Summary of notes: "
		<< aoc::sum<size_t>(patterns, pattern_reflection<0>)
		<< std::endl;

	std::cout << "(Part 2) Summary of notes: "
		<< aoc::sum<size_t>(patterns, pattern_reflection<1>)
		<< std::endl;

	return EXIT_SUCCESS;
}

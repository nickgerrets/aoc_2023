#include "common.h"
#include "vec2.h"

#include <vector>
#include <sstream>

struct Instruction {
	char direction;
	uint64_t count;
	char true_dir;
	uint64_t true_count;
};

std::vector<Instruction> parse_instructions(std::istream& stream) {
	std::vector<Instruction> instructions;
	for (auto const& l : aoc::Lines(stream)) {

		Instruction inst;

		std::istringstream ss(l);
		std::string hex;
		ss >> inst.direction >> inst.count >> hex;

		inst.true_count = std::stoull(hex.substr(2, 5), 0, 16);
		char c = hex[hex.length() - 2];
		switch (c) {
			case '0': c = 'R'; break;
			case '1': c = 'D'; break;
			case '2': c = 'L'; break;
			case '3': c = 'U'; break;
		}
		inst.true_dir = c;

		instructions.push_back(inst);
	}
	return instructions;
}

Vec2 dir_from_c(char c) {
	switch (c) {
		case 'R': return {1, 0};
		case 'D': return {0, 1};
		case 'L': return {-1, 0};
		case 'U': return {0, -1};
	}
	return {0, 0};
}

int64_t polygonal_area(std::vector<Vec2> const& lines) {
	// https://web.archive.org/web/20100405070507/http://valis.cs.uiuc.edu/~sariel/research/CG/compgeom/msg00831.html
	double area;
	for (size_t i = 0; i < lines.size() - 1; ++i) {
		size_t j = (i + 1) % lines.size();
		area += lines[i].x * lines[j].y;
		area -= lines[j].x * lines[i].y;
	}
	area = std::abs(area) * 0.5;
	return static_cast<int64_t>(area);
}

template <bool PART2 = false>
int64_t calculate_area(std::vector<Instruction> const& instructions) {
	std::vector<Vec2> lines;
	int64_t total_length = 0;

	// cursor
	Vec2 cursor {0, 0};
	for (auto const& i : instructions) {
		char c = PART2 ? i.true_dir : i.direction;

		Vec2 dir = dir_from_c(c);

		int64_t len = PART2 ? i.true_count : i.count;

		cursor = cursor + (dir * len);

		lines.push_back(cursor);
		total_length += len;
	}
	return polygonal_area(lines) + total_length / 2 + 1;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto instructions = parse_instructions(*input);

	std::cout << "(Part 1) Area: " << calculate_area(instructions) << std::endl;
	std::cout << "(Part 1) Area: " << calculate_area<true>(instructions) << std::endl;

	return (EXIT_SUCCESS);
}

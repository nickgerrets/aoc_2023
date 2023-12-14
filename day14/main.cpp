#include "common.h"

#include <vector>
#include <sstream>
#include <unordered_map>

using grid_t = std::vector<std::string>;

grid_t parse_rocks(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return grid_t(lines.begin(), lines.end());
}

int64_t move_rock(grid_t& rocks, int64_t x, int64_t y, int64_t dx, int64_t dy) {
	while (x + dx >= 0 && y + dy >= 0 && x + dx < rocks[0].length() && y + dy < rocks.size()) {
		if (rocks[y + dy][x + dx] == '.') {
			rocks[y][x] = '.';
			rocks[y + dy][x + dx] = 'O';
		} else {
			break ;
		}
		x += dx;
		y += dy;
	}
	return (rocks.size() - y);
}

int64_t move_rocks_dir(grid_t& rocks, int64_t dx, int64_t dy) {
	int64_t load = 0;

	int64_t xstart = dx <= 0 ? 0 : rocks[0].length() - 1;
	int64_t ystart = dy <= 0 ? 0 : rocks.size() - 1;
	int64_t xinc = dx <= 0 ? 1 : -1;
	int64_t yinc = dy <= 0 ? 1 : -1;

	for (int64_t y = ystart; y >= 0 && y < rocks.size(); y += yinc) {
		for (int64_t x = xstart; x >= 0 && x < rocks[0].length(); x += xinc) {
			if (rocks[y][x] == 'O') {
				load += move_rock(rocks, x, y, dx, dy);
			}
		}
	}
	return load;
}

// For hashing the grid
std::string grid_to_string(grid_t const& g) {
	std::string str;
	for (auto const& row : g) {
		str += row;
	}
	return str;
}

int64_t do_cycles(grid_t& rocks) {
	static size_t const CYCLES = 1000000000;

	int64_t load = 0;
	std::unordered_map<std::string, size_t> cache;

	size_t i = 0;
	bool found = false;
	while (i++ < CYCLES) {
		// Do the cycle
		move_rocks_dir(rocks, 0, -1); // N
		move_rocks_dir(rocks, -1, 0); // W
		move_rocks_dir(rocks, 0, 1); // S
		load = move_rocks_dir(rocks, 1, 0); // E

		std::string grid_hash = grid_to_string(rocks);
		// Check if we have cached this cycle
		if (cache.find(grid_hash) != cache.end()) {
			size_t diff = i - cache[grid_hash]; // How many cycles can we jump forward?
			i += ((CYCLES - i) / diff) * diff; // Jump forward as 'many' times as we can
		} else {
			cache[grid_hash] = i; // cache the result
		}

	}
	return load;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto rocks = parse_rocks(*input);

	int64_t load = move_rocks_dir(rocks, 0, -1); // NORTH
	std::cout << "(Part 1) Load on north support beam: " << load << std::endl;

	load = do_cycles(rocks);
	std::cout << "(Part 2) Load on north support beam: " << load << std::endl;

	return EXIT_SUCCESS;
}

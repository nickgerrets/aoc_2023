#include "common.h"

#include <vector>
#include <unordered_set>
#include <set>
#include <stack>

using grid_t = std::vector<std::string>;

grid_t parse_grid(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return grid_t(lines.begin(), lines.end());
}

struct Pos {
	int64_t x, y;

	bool operator==(Pos const& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}

	// For set
	bool operator<(Pos const& rhs) const {
		if (y == rhs.y) {
			return (x < rhs.x);
		}
		return (y < rhs.y);
	}

	std::string to_string() const {
		return (std::to_string(x) + std::to_string(y));
	}
};

struct Beam {
	Pos p, d;

	bool operator==(Beam const& rhs) const {
		return (p == rhs.p && d == rhs.d);
	}
};

// For unordered_set
template <>
struct std::hash<Beam> {
	size_t operator()(Beam const& b) const {
		return std::hash<std::string>()(b.p.to_string() + b.d.to_string());
	}
};

std::vector<Beam> move_beam(Beam curr, char c) {
	switch (c) {
		case '-': {
			if (curr.d.y != 0) {
				// split beam
				return { { curr.p, {-1, 0} }, { curr.p, {1, 0}} };
			}
			break ;
		}
		case '|': {
			if (curr.d.x != 0) {
				// split beam
				return { { curr.p, {0, -1} }, { curr.p, {0, 1}} };
			}
			break ;
		}
		case '/': {
			// rotate self
			curr.d = {-curr.d.y, -curr.d.x};
			break ;
		}
		case '\\': {
			curr.d = {curr.d.y, curr.d.x};
			break ;
		}
	}
	curr.p = {curr.p.x + curr.d.x, curr.p.y + curr.d.y};
	return {curr};
}

int64_t solve(grid_t & grid, Beam start) {
	std::unordered_set<Beam> energized;
	energized.insert(start);

	std::stack<Beam> beams;
	beams.push(start);
	while (!beams.empty()) {
		Beam curr = beams.top(); beams.pop();
		energized.insert(curr);

		auto new_beams = move_beam(curr, grid[curr.p.y][curr.p.x]);
		for (auto b : new_beams) {
			if (energized.count(b) > 0 ||
			    b.p.x < 0 || b.p.y < 0 || b.p.x >= grid[0].length() || b.p.y >= grid.size()) {
				continue ;
			}
			beams.push(b);
		}
	}

	// Only take unique positions
	std::set<Pos> unique;
	for (auto const& e : energized) {
		unique.insert(e.p);
	}
	return unique.size();
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto grid = parse_grid(*input);

	std::cout << "(Part 1) Energized tiles: " << solve(grid, {{0, 0}, {1, 0}}) << std::endl;

	// Part 2
	// Do the same but for every column/row and then get the max
	int64_t energized = 0;
	for (int64_t y = 0; y < grid.size(); ++y) {
		int64_t maxy = std::max(
			solve(grid, {{0, y}, {1, 0}}),
			solve(grid, {{(int64_t)grid[0].length() - 1, y}, {-1, 0}}));
		energized = std::max(energized, maxy);
	}

	for (int64_t x = 0; x < grid[0].length(); ++x) {
		int64_t maxx = std::max(
			solve(grid, {{x, 0}, {0, 1}}),
			solve(grid, {{x, (int64_t)grid.size() - 1}, {0, -1}}));
		energized = std::max(energized, maxx);
	}

	std::cout << "(Part 2) Energized tiles: " << energized << std::endl;

	return EXIT_SUCCESS;
}

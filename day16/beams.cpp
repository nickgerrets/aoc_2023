#include "common.h"
#include "vec2.h"

#include <vector>
#include <unordered_set>
#include <set>
#include <stack>

using grid_t = std::vector<std::string>;

grid_t parse_grid(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return grid_t(lines.begin(), lines.end());
}

// For set
template <>
bool Vec2::operator<(Vec2 const& rhs) const {
	if (y == rhs.y) {
		return (x < rhs.x);
	}
	return (y < rhs.y);
}

struct Beam {
	Vec2 p, d;

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
				return { { curr.p, Vec2::left() }, { curr.p, Vec2::right() } };
			}
			break ;
		}
		case '|': {
			if (curr.d.x != 0) {
				// split beam
				return { { curr.p, Vec2::up() }, { curr.p, Vec2::down() } };
			}
			break ;
		}
		case '/': {
			// rotate self
			curr.d = { -curr.d.y, -curr.d.x };
			break ;
		}
		case '\\': {
			curr.d = { curr.d.y, curr.d.x };
			break ;
		}
	}
	curr.p = curr.p + curr.d;
	return { curr };
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
			    !b.p.is_within_bounds({}, Vec2(grid[0].length() - 1, grid.size() - 1))) {
				continue ;
			}
			beams.push(b);
		}
	}

	// Only take unique positions
	std::set<Vec2> unique;
	for (auto const& e : energized) {
		unique.insert(e.p);
	}
	return unique.size();
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto grid = parse_grid(*input);

	std::cout << "(Part 1) Energized tiles: " << solve(grid, {{0, 0}, Vec2::right()}) << std::endl;

	// Part 2
	// Do the same but for every column/row and then get the max
	int64_t energized = 0;
	for (int64_t y = 0; y < grid.size(); ++y) {
		int64_t maxy = std::max(
			solve(grid, {{0, y}, Vec2::right()}),
			solve(grid, {{(int64_t)grid[0].length() - 1, y}, Vec2::left()}));
		energized = std::max(energized, maxy);
	}

	for (int64_t x = 0; x < grid[0].length(); ++x) {
		int64_t maxx = std::max(
			solve(grid, {{x, 0}, Vec2::down()}),
			solve(grid, {{x, (int64_t)grid.size() - 1}, Vec2::up()}));
		energized = std::max(energized, maxx);
	}

	std::cout << "(Part 2) Energized tiles: " << energized << std::endl;

	return EXIT_SUCCESS;
}

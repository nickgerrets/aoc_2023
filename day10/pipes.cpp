#include "common.h"
#include "vec2.h"

#include <vector>
#include <stack>
#include <unordered_map>

enum Location {
	NONE = 0,
	OUTSIDE,
	INSIDE,
	PATH
};

using loc_map_t = std::vector<std::vector<Location>>;
using pipe_map_t = std::vector<std::string>;

pipe_map_t parse_map(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return pipe_map_t(lines.begin(), lines.end());
}

Vec2 find_start(pipe_map_t const& map) {
	for (int64_t y = 0; y < map.size(); ++y) {
		for (int64_t x = 0; x < map[0].size(); ++x) {
			if (map[y][x] == 'S') {
				return {x, y};
			}
		}
	}
	return {0, 0};
}

/*
	| is a vertical pipe connecting north and south.
	- is a horizontal pipe connecting east and west.
	L is a 90-degree bend connecting north and east.
	J is a 90-degree bend connecting north and west.
	7 is a 90-degree bend connecting south and west.
	F is a 90-degree bend connecting south and east.
	. is ground; there is no pipe in this tile.
	S is the starting position (connecting all 4 directions)
*/
static std::unordered_map<char, std::vector<Vec2>> const DIRECTION_MAP = {
	{ 'S', {Vec2::up(), Vec2::down(), Vec2::left(), Vec2::right()} },
	{ '|', {Vec2::up(), Vec2::down()} },
	{ '-', {Vec2::left(), Vec2::right()} },
	{ 'L', {Vec2::up(), Vec2::right()} },
	{ 'J', {Vec2::up(), Vec2::left()} },
	{ '7', {Vec2::down(), Vec2::left()} },
	{ 'F', {Vec2::down(), Vec2::right()} },
	{ '.', {}}
};

std::vector<Vec2> get_connections(pipe_map_t const& map, Vec2 p) {
	Vec2 const END_POS = Vec2(map[0].length() - 1, map.size() - 1);
	// Bounds check
	if (!p.is_within_bounds({0, 0}, END_POS)) {
		return {};
	}

	std::vector<Vec2> connections;
	for (Vec2 const& d : DIRECTION_MAP.at(map[p.y][p.x])) {
		Vec2 d_abs = d + p; // to absolute position
		if (!d_abs.is_within_bounds({0, 0}, END_POS)) {
			continue;
		}

		auto next_dirs = DIRECTION_MAP.at(map[d_abs.y][d_abs.x]);
		for (Vec2& n : next_dirs) {
			if (n + d_abs == p) {
				connections.push_back(d_abs);
				break ;
			}
		}
	}
	return connections;
}

// Trace a path from S through the loop back to S
std::vector<Vec2> find_path(pipe_map_t const& map) {
	Vec2 start = find_start(map);

	// For every connection coming from start
	auto connections = get_connections(map, start);
	for (Vec2& current : connections) {
		std::vector<Vec2> path;
		path.push_back(start);
		path.push_back(current);

		Vec2 from = start;
		// trace a path
		for (auto to = get_connections(map, current); to.size() == 2; to = get_connections(map, current)) {
			path.push_back( (to[0] == from) ? to[1] : to[0] );
			from = current;
			current = path.back();
			// Until we reach start
			if (current == start) {
				return path;
			}
		}
	}
	return {}; // no path
}

void draw_path(loc_map_t& map, std::vector<Vec2> const& path) {
	for (auto const& p : path) {
		map[p.y][p.x] = PATH;
	}
}

// Fill the map with INSIDE/OUTSIDE and return amount of INSIDE
size_t calculate_inside(loc_map_t& result_map, pipe_map_t const& pipe_map) {
	size_t amount = 0;
	for (int64_t y = 0; y < result_map.size(); ++y) {
		for (int64_t x = 0; x < result_map[0].size(); ++x) {
			auto& loc = result_map[y][x];
			size_t count = 0;
			if (loc != NONE) {
				continue;
			}
			for (size_t i = 0; i < x; ++i) {
				char o = pipe_map[y][i];
				if (result_map[y][i] == PATH && (o == '|' || o =='J' || o == 'L')) {
					++count;
				}
			}
			if (count % 2 != 0) {
				loc = INSIDE;
				++amount;
			} else {
				loc = OUTSIDE;
			}
		}
	}
	return amount;
}

void determine_start_char(pipe_map_t& pipe_map, std::vector<Vec2> const& path) {
	assert(path.size() > 1);

	Vec2 start = find_start(pipe_map);
	Vec2 pos_up = path[1] - start;
	Vec2 pos_down = path[path.size() - 2] - start;

	if (pos_up.y >= 0 && pos_down.y >= 0) {
		return ; // Not interested
	}

	if (pos_up.y > pos_down.y) {
		std::swap(pos_up, pos_down);
	}

	char c = '|';
	if (pos_down.x > 0) {
		c = 'L';
	} else if (pos_down.x < 0) {
		c = 'J';
	}
	pipe_map[start.y][start.x] = c;
}

/*
void debug_map_draw(loc_map_t const& result_map) {
	for (auto const& row : result_map) {
		for (auto const& x : row) {
			if (x == OUTSIDE) {
				std::cout << ' ';
			} else if (x == INSIDE) {
				std::cout << '.';
			} else if (x == PATH) {
				std::cout << '#';
			}
		}
		std::cout << std::endl;
	}
}
*/

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto map = parse_map(*input);
	auto path = find_path(map);

	std::cout << "(Part 1) Steps from start to farthest point: " << path.size() / 2 << std::endl;

	// Part 2
	loc_map_t result_map(map.size(), std::vector<Location>(map[0].size(), NONE));

	draw_path(result_map, path);
	determine_start_char(map, path);
	size_t inside = calculate_inside(result_map, map);

	// debug_map_draw(result_map);
	std::cout << "(Part 2) Tiles enclosed by the loop: " << inside << std::endl;

	return (EXIT_FAILURE);
}

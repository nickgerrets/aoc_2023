#include "common.h"

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

struct Pos {
	int64_t y;
	int64_t x;

	Pos operator+(Pos const& rhs) const {
		return {.y = y + rhs.y, .x = x + rhs.x};
	}

	Pos operator-(Pos const& rhs) const {
		return {.y = y - rhs.y, .x = x - rhs.x};
	}

	bool operator==(Pos const& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}

	bool operator!=(Pos const& rhs) const {
		return !(*this == rhs);
	}
};

using pipe_map_t = std::vector<std::string>;

pipe_map_t parse_map(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return pipe_map_t(lines.begin(), lines.end());
}

Pos find_start(pipe_map_t const& map) {
	for (int64_t y = 0; y < map.size(); ++y) {
		for (int64_t x = 0; x < map[0].size(); ++x) {
			if (map[y][x] == 'S') {
				return {y, x};
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
static std::unordered_map<char, std::vector<Pos>> const DIRECTION_MAP = {
	{ 'S', {{-1, 0}, {1, 0}, {0, -1}, {0, 1}} },
	{ '|', {{-1, 0}, {1, 0}} },
	{ '-', {{0, -1}, {0, 1}} },
	{ 'L', {{-1, 0}, {0, 1}} },
	{ 'J', {{-1, 0}, {0, -1}} },
	{ '7', {{1, 0}, {0, -1}} },
	{ 'F', {{1, 0}, {0, 1}} },
	{ '.', {}}
};

bool is_within_bounds(pipe_map_t const& map, Pos p) {
	if (p.y < 0 || p.y >= map.size() || p.x < 0 || p.x >= map[0].size()) {
		return false;
	}
	return true;
}

std::vector<Pos> get_connections(pipe_map_t const& map, Pos p) {
	// Bounds check
	if (!is_within_bounds(map, p)) {
		return {};
	}

	std::vector<Pos> connections;
	for (Pos const& d : DIRECTION_MAP.at(map[p.y][p.x])) {
		Pos d_abs = d + p; // to absolute position
		if (!is_within_bounds(map, d_abs)) {
			continue;
		}

		auto next_dirs = DIRECTION_MAP.at(map[d_abs.y][d_abs.x]);

		for (Pos& n : next_dirs) {
			if (n + d_abs == p) {
				connections.push_back(d_abs);
				break ;
			}
		}
	}
	return connections;
}

// Trace a path from S through the loop back to S
std::vector<Pos> find_path(pipe_map_t const& map) {
	Pos start = find_start(map);

	// For every connection coming from start
	auto connections = get_connections(map, start);
	for (Pos& current : connections) {
		std::vector<Pos> path;
		path.push_back(start);
		path.push_back(current);

		Pos from = start;
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

void draw_path(loc_map_t& map, std::vector<Pos> const& path) {
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

void determine_start_char(pipe_map_t& pipe_map, std::vector<Pos> const& path) {
	assert(path.size() > 1);

	Pos start = find_start(pipe_map);
	Pos pos_up = path[1] - start;
	Pos pos_down = path[path.size() - 2] - start;

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

#include "common.h"

#include <vector>
#include <stack>

enum Location {
	NONE = 0,
	OUTSIDE,
	INSIDE,
	PATH
};

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

using map_t = std::vector<std::vector<int64_t>>;

map_t parse_map(std::istream& stream) {
	map_t map;
	for (auto const& line : aoc::Lines(stream)) {
		map.emplace_back(line.begin(), line.end());
	}
	return map;
}

Pos find_start(map_t const& map) {
	for (int64_t y = 0; y < map.size(); ++y) {
		for (int64_t x = 0; x < map[0].size(); ++x) {
			if (map[y][x] == 'S') {
				return Pos {y, x};
			}
		}
	}
	return Pos {0, 0};
}

/*
	| is a vertical pipe connecting north and south.
	- is a horizontal pipe connecting east and west.
	L is a 90-degree bend connecting north and east.
	J is a 90-degree bend connecting north and west.
	7 is a 90-degree bend connecting south and west.
	F is a 90-degree bend connecting south and east.
	. is ground; there is no pipe in this tile.
	S is the starting position
*/
std::vector<Pos> get_directions(char c) {
	std::vector<Pos> p;
	switch (c) {
		case 'S':
			p.push_back( {-1, 0} );
			p.push_back( {1, 0} );
			p.push_back( {0, -1} );
			p.push_back( {0, 1} );
			break;
		case '|':
			p.push_back( {-1, 0} );
			p.push_back( {1, 0} );
			break;
		case '-':
			p.push_back( {0, -1} );
			p.push_back( {0, 1} );
			break;
		case 'L':
			p.push_back( {-1, 0} );
			p.push_back( {0, 1} );
			break;
		case 'J':
			p.push_back( {-1, 0} );
			p.push_back( {0, -1} );
			break;
		case '7':
			p.push_back( {1, 0} );
			p.push_back( {0, -1} );
			break;
		case 'F':
			p.push_back( {1, 0} );
			p.push_back( {0, 1} );
			break;
	}
	return p;
}

bool is_within_bounds(map_t const& map, Pos p) {
	if (p.y < 0 || p.y >= map.size() || p.x < 0 || p.x >= map[0].size()) {
		return false;
	}
	return true;
}

std::vector<Pos> get_connections(map_t const& map, Pos p) {
	// Bounds check
	if (!is_within_bounds(map, p)) {
		return {};
	}

	std::vector<Pos> connections;
	auto dirs = get_directions(map[p.y][p.x]);
	for (Pos const& d : dirs) {
		Pos d_abs = d + p; // to absolute position
		if (!is_within_bounds(map, d_abs)) {
			continue;
		}

		auto next_dirs = get_directions(map[d_abs.y][d_abs.x]);

		for (Pos& n : next_dirs) {
			if (n + d_abs == p) {
				connections.push_back(d + p);
				break ;
			}
		}
	}
	return connections;
}

std::vector<Pos> find_path(map_t const& map) {
	Pos start = find_start(map);

	// For every connection coming from start
	auto connections = get_connections(map, start);
	for (Pos& current : connections) {
		std::vector<Pos> path;
		path.push_back(start);
		path.push_back(current);

		Pos from = start;
		// trace a path
		while (true) {
			auto to = get_connections(map, current);
			if (to.size() == 2) {
				if (to[0] == from) {
					path.push_back(to[1]);
				} else {
					path.push_back(to[0]);
				}
			} else {
				break ;
			}
			from = current;
			current = path.back();
			if (current == start) {
				return path;
			}
		}
	}
	return {};
}

void draw_path(map_t& map, std::vector<Pos> const& path) {
	for (auto const& p : path) {
		map[p.y][p.x] = PATH;
	}
}

size_t calculate_inside(map_t& result_map, map_t const& orig_map) {
	size_t amount = 0;
	for (int64_t y = 0; y < result_map.size(); ++y) {
		for (int64_t x = 0; x < result_map[0].size(); ++x) {
			int64_t& c = result_map[y][x];
			size_t count = 0;
			if (c != NONE) {
				continue;
			}
			for (size_t i = 0; i < x; ++i) {
				char o = orig_map[y][i];
				if (result_map[y][i] == PATH && (o == '|' || o =='J' || o == 'L')) {
					++count;
				}
			}
			if (count % 2 != 0) {
				c = INSIDE;
				++amount;
			} else {
				c = OUTSIDE;
			}
		}
	}
	return amount;
}

void determine_start_char(map_t& orig_map, std::vector<Pos> const& path) {

	Pos start = find_start(orig_map);
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
	orig_map[start.y][start.x] = c;
}

void debug_map_draw(map_t const& result_map) {
	for (auto const& y : result_map) {
		for (auto const& x : y) {
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
	map_t result_map(map.size(), std::vector<int64_t>(map[0].size(), NONE));

	draw_path(result_map, path);
	determine_start_char(map, path);
	size_t inside = calculate_inside(result_map, map);

	std::cout << "(Part 2) Tiles enclosed by the loop: " << inside << std::endl;

	return (EXIT_FAILURE);
}

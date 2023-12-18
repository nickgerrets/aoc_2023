#include "common.h"
#include "vec2.h"

#include <vector>
#include <queue>
#include <unordered_map>

using grid_t = std::vector<std::string>;

grid_t parse_grid(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return grid_t(lines.begin(), lines.end());
}

struct Permutation {
	Vec2 pos;
	Vec2 dir;
	int64_t count;

	bool operator==(Permutation const& rhs) const {
		return (pos == rhs.pos && dir == rhs.dir && count == rhs.count);
	}

	std::string to_string() const {
		return (pos.to_string() + dir.to_string() + std::to_string(count));
	}

	size_t hash() const {
		return std::hash<std::string>()(to_string());
	}
};

// Hash permutations for the distance map
template <>
struct std::hash<Permutation> {
	size_t operator()(Permutation const& d) const {
		return d.hash();
	}
};

struct Data {
	size_t cost;
	Permutation data;

	bool operator<(Data const& rhs) const {
		return cost > rhs.cost; // needs to be sorted based on cost being higher
	}

	bool operator==(Data const& rhs) const {
		return (cost == rhs.cost && data == rhs.data);
	}

};

int64_t get_min(std::unordered_map<Permutation, int64_t> const& distances, Vec2 const end) {
	int64_t min_distance = INT64_MAX;
	for (auto const& p : distances) {
		if (p.first.pos == end) {
			min_distance = std::min(min_distance, p.second);
		}
	}
	return min_distance;
}

template <bool PART2>
int64_t solve(grid_t const& grid) {
	Vec2 const END_POS (grid[0].length() - 1, grid.size() - 1);

	std::priority_queue<Data> q;
	q.push({0, {{}, Vec2::right(), 0}}); // RIGHT
	q.push({0, {{}, Vec2::down(), 0}}); // DOWN

	std::unordered_map<Permutation, int64_t> distances;

	while (!q.empty()) {
		Data q_current = q.top(); q.pop();
		Permutation& curr = q_current.data;

		if (curr.pos == END_POS) {
			return q_current.cost;
		}
		
		if (distances.find(curr) != distances.end()) {
			continue;
		}

		distances[curr] = q_current.cost;

		std::initializer_list<Vec2> DIRS {Vec2::right(), Vec2::up(), Vec2::left(), Vec2::down()};
		for (auto const& new_dir : DIRS) {
			// Can't reverse
			if (curr.dir == -new_dir) {
				continue ;
			}

			Vec2 new_pos = curr.pos + new_dir;
			// bounds check
			if (!new_pos.is_within_bounds({0, 0}, END_POS)) {
				continue ;
			}

			int64_t new_count = (new_dir != curr.dir) ? 1 : curr.count + 1;

			// max of 3 in same direction
			if (!PART2 && new_count > 3) {
				continue ;
			} else if (PART2 && !(new_count <= 10 && (new_dir == curr.dir || curr.count >= 4))) {
				continue ;
			}

			char cost = grid[new_pos.y][new_pos.x] - '0';
			q.push({q_current.cost + cost, {new_pos, new_dir, new_count}});
		}

	}
	return get_min(distances, END_POS);
}

int main(int argc, char**argv) {
	auto input = aoc::get_input(argc, argv);

	auto grid = parse_grid(*input);

	std::cout << "(Part 1) Least heat loss: " << solve<false>(grid) << std::endl;
	std::cout << "(Part 2) Least heat loss: " << solve<true>(grid) << std::endl;

	return EXIT_SUCCESS;
}

#include "common.h"

#include <vector>
#include <set>

struct Pos {
	int64_t y;
	int64_t x;

	int64_t distance(Pos const& other) const {
		// "manhattan distance"
		return (std::abs(y - other.y) + std::abs(x - other.x));
	}
};

static char const GALAXY_CHAR = '#';

std::set<size_t> get_empty_columns(std::vector<std::string> const& lines) {
	std::set<size_t> empty;

	auto check_column = [lines](size_t i){
		for (auto const& l : lines) {
			if (l[i] == GALAXY_CHAR) {
				return false;
			}
		}
		return true;
	};

	for (size_t i = 0; i < lines[0].length(); ++i) {
		if (check_column(i)) {
			empty.insert(i);
		}
	}
	return empty;
}

std::vector<std::string> parse_lines(std::istream& stream) {
	auto lines = aoc::Lines(stream);
	return std::vector<std::string>(lines.begin(), lines.end());
}

std::vector<Pos> map_galaxies(std::vector<std::string> const& lines, int64_t const expansion = 2) {
	auto empty_columns = get_empty_columns(lines);

	std::vector<Pos> galaxies;
	int64_t y = 0;
	for (int64_t yi = 0; yi < lines.size(); ++yi) {
		if (lines[yi].find_first_of(GALAXY_CHAR) == std::string::npos) {
			y += (expansion - 1); // this -1 is important, was stuck on this for a while
			continue;
		}

		int64_t x = 0;
		for (int64_t xi = 0; xi < lines[0].size(); ++xi) {
			if (empty_columns.count(xi) > 0) {
				x += (expansion - 1);
				continue ;
			}

			if (lines[yi][xi] == GALAXY_CHAR) {
				galaxies.push_back({.y = y + yi, .x = x + xi});
			}
		}
	}
	return galaxies;
}

std::vector<std::pair<Pos, Pos>> get_pairs(std::vector<Pos> const& galaxies) {
	std::vector<std::pair<Pos, Pos>> pairs;
	for (auto it1 = galaxies.begin(); it1 != galaxies.end(); ++it1) {
		for (auto it2 = it1 + 1; it2 != galaxies.end(); ++it2) {
			pairs.emplace_back(*it1, *it2);
		}
	}
	return (pairs);
}

int64_t distance(std::pair<Pos, Pos> const& pair) {
	return pair.first.distance(pair.second);
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);
	auto lines = parse_lines(*input);

	auto galaxies = map_galaxies(lines);
	auto pairs = get_pairs(galaxies);

	std::cout << "(Part 1) Sum of lengths of shortest paths between galaxies: "
		<< aoc::sum<int64_t>(pairs, distance)
		<< std::endl;

	// Part 2, remap galaxies with bigger expansion
	galaxies = map_galaxies(lines, 1e6);
	pairs = get_pairs(galaxies);	
	std::cout << "(Part 2) Sum of lengths of shortest paths between galaxies: "
		<< aoc::sum<int64_t>(pairs, distance)
		<< std::endl;

	return (EXIT_SUCCESS);
}
